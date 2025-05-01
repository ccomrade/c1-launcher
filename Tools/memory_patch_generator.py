from enum import StrEnum
from hashlib import sha256
from pathlib import Path

from iced_x86 import Code, Encoder, Instruction, MemoryOperand, Register
from pefile import PE

from checksums import CRY_CHECKSUMS
from disassembler import Disassembler, Subroutine

CRYSIS_BUILDS = (5767, 5879, 6115, 6156)
CRYSIS_WARS_64BIT_BUILDS = (6566, 6586, 6627, 6670, 6729)
CRYSIS_WARS_32BIT_BUILDS = (6527,) + CRYSIS_WARS_64BIT_BUILDS
CRYSIS_WARS_BUILDS = CRYSIS_WARS_32BIT_BUILDS
CRYSIS_WARHEAD_64BIT_BUILDS = (710, 711)
CRYSIS_WARHEAD_32BIT_BUILDS = (687,) + CRYSIS_WARHEAD_64BIT_BUILDS
CRYSIS_WARHEAD_BUILDS = CRYSIS_WARHEAD_32BIT_BUILDS

class Subsystem(StrEnum):
    Cry3DEngine = 'Cry3DEngine.dll'
    CryAISystem = 'CryAISystem.dll'
    CryAction = 'CryAction.dll'
    CryAnimation = 'CryAnimation.dll'
    CryEntitySystem = 'CryEntitySystem.dll'
    CryFont = 'CryFont.dll'
    CryGame = 'CryGame.dll'
    CryInput = 'CryInput.dll'
    CryMovie = 'CryMovie.dll'
    CryNetwork = 'CryNetwork.dll'
    CryPhysics = 'CryPhysics.dll'
    CryRenderD3D10 = 'CryRenderD3D10.dll'
    CryRenderD3D9 = 'CryRenderD3D9.dll'
    CryRenderNULL = 'CryRenderNULL.dll'
    CryScriptSystem = 'CryScriptSystem.dll'
    CrySoundSystem = 'CrySoundSystem.dll'
    CrySystem = 'CrySystem.dll'

class CryDll:
    def __init__(self, path: Path, subsystem: Subsystem, pe: PE, build: int, disasm: Disassembler):
        self.path = path
        self.subsystem = subsystem
        self.pe = pe
        self.build = build
        self.disasm = disasm

    @property
    def x64(self) -> bool:
        return self.disasm.x64

    @property
    def bits(self) -> int:
        return 64 if self.x64 else 32

    @property
    def is_og(self) -> bool:
        return self.build in CRYSIS_BUILDS

    @property
    def is_wars(self) -> bool:
        return self.build in CRYSIS_WARS_BUILDS

    @property
    def is_warhead(self) -> bool:
        return self.build in CRYSIS_WARHEAD_BUILDS

def get_build_dir(build: int) -> str:
    if build in CRYSIS_BUILDS:
        return f'Crysis_{build}'
    if build in CRYSIS_WARS_BUILDS:
        return f'Crysis_Wars_{build}'
    if build in CRYSIS_WARHEAD_BUILDS:
        return f'Crysis_Warhead_{build}'
    assert False, f'Unknown build {build}'

def get_dll_path(subsystem: Subsystem, build: int, x64: bool) -> Path:
    base_path = Path(__file__).with_name('DLLs')
    build_dir = get_build_dir(build)
    bin_dir = 'Bin64' if x64 else 'Bin32'
    filename = subsystem.value
    if build in CRYSIS_WARHEAD_BUILDS and subsystem in (Subsystem.CryAction, Subsystem.CryGame):
        filename = 'Crysis64.exe'
    return base_path / build_dir / bin_dir / filename

def get_real_dll_build(dll: CryDll) -> int:
    build = dll.pe.VS_FIXEDFILEINFO[0].ProductVersionLS & 0xffff
    if build == 1 and dll.subsystem is Subsystem.CryRenderD3D10:
        build = 5767
    return build

def verify_dll_checksum(dll: CryDll):
    build = dll.build
    bits = dll.bits
    name = dll.path.name
    checksum = sha256(dll.path.read_bytes()).hexdigest()
    assert build in CRY_CHECKSUMS, f'Unknown build {build} of {dll.path}'
    assert bits in CRY_CHECKSUMS[build], f'Unknown {bits}-bit variant of {dll.path}'
    assert name in CRY_CHECKSUMS[build][bits], f'Unknown name "{name}" of {dll.path}'
    assert checksum == CRY_CHECKSUMS[build][bits][name], f'Invalid checksum of {dll.path}'

def load_dll(subsystem: Subsystem, build: int, x64: bool) -> CryDll:
    path = get_dll_path(subsystem, build, x64)
    print(f'Loading {path}')
    pe = PE(path)
    disasm = Disassembler(pe)
    dll = CryDll(path, subsystem, pe, build, disasm)
    assert dll.x64 == x64
    assert get_real_dll_build(dll) == build
    verify_dll_checksum(dll)
    return dll

class Context:
    def __init__(self, subsystem: Subsystem, dlls: list[CryDll]):
        self.subsystem = subsystem
        self.dlls = dlls

    @classmethod
    def load(cls, subsystem: Subsystem, full: bool):
        dlls = []
        if full:
            # 32-bit Crysis Warhead is not supported
            for build in CRYSIS_BUILDS + CRYSIS_WARS_32BIT_BUILDS:
                dlls.append(load_dll(subsystem, build, x64=False))
            for build in CRYSIS_BUILDS + CRYSIS_WARS_64BIT_BUILDS + CRYSIS_WARHEAD_64BIT_BUILDS:
                dlls.append(load_dll(subsystem, build, x64=True))
        else:
            dlls.append(load_dll(subsystem, build=6156, x64=False))
            dlls.append(load_dll(subsystem, build=6156, x64=True))
        return cls(subsystem, dlls)

class Assembly:
    def __init__(self, x64: bool):
        self.encoder = Encoder(64 if x64 else 32)
        self.instructions: list[Instruction] = []
        self.machine_code: list[bytes] = []
        self.code_size = 0

    def add(self, instruction: Instruction):
        self.instructions.append(instruction)
        self.code_size += self.encoder.encode(instruction, rip=0)
        self.machine_code.append(self.encoder.take_buffer())

class CodeGenerator:
    INDENT = '\t'
    MAX_LINE_LENGTH = 120

    def __init__(self):
        self.indent_level = 0
        self.lines: list[str] = []

    def add(self, line: str, *, no_indent = False):
        line = (self.INDENT * self.indent_level * (not no_indent)) + line if line else ''
        assert len(line) <= self.MAX_LINE_LENGTH, f'Too long line: "{line}"'
        self.lines.append(line)

    def begin_block(self, line = '{'):
        self.add(line)
        self.indent_level += 1

    def end_block(self, line = '}'):
        self.indent_level -= 1
        self.add(line)

    def begin_function(self, ret_type: str, name: str, params: list[str]):
        line = f'{ret_type} {name}('
        for index, param in enumerate(params):
            if len(line + param) > (self.MAX_LINE_LENGTH - 3):
                self.add(line + (',' if index > 0 else ''))
                line = ''
            if index > 0 and line:
                line += ', '
            if not line:
                line += self.INDENT
            line += param
        self.add(line + ')')
        self.begin_block()

    def end_function(self):
        self.end_block()

    def add_doc_comment(self, comment: str):
        self.add('/**')
        if comment:
            lines = comment.splitlines()
            # remove trailing empty line
            if not lines[-1].strip():
                lines.pop()
            for line in lines:
                self.add(f' * {line.lstrip()}'.rstrip())
        self.add(' */')

    def add_build_switch_cases(self, cases: dict[int, list[str]]):
        builds = list(cases)
        builds.sort()
        for i, build in enumerate(builds):
            self.add(f'case {build}:')
            if (i + 1) < len(builds) and cases[build] == cases[builds[i + 1]]:
                # merge identical adjacent cases
                continue
            self.begin_block()
            for line in cases[build]:
                self.add(line)
                self.add('break;')
            self.end_block()

    def add_assembly_as_c_array(self, assembly: Assembly, name: str, const: bool):
        max_length = max(len(data) for data in assembly.machine_code)
        self.begin_block(('const ' if const else '') + f'unsigned char {name}[] = ' + '{')
        for instruction, data in zip(assembly.instructions, assembly.machine_code):
            line = ''.join(f'0x{byte:02x}, ' for byte in data)
            line += ' ' * ((max_length - len(data)) * 6)
            line += f' // {instruction:xrsSMi}'
            self.add(line)
        self.end_block('};')

class DllSubroutine:
    def __init__(self, dll: CryDll, subroutine: Subroutine):
        self.dll = dll
        self.subroutine = subroutine

class MemoryPatch:
    def __init__(self, subsystem: Subsystem, name: str):
        self.subsystem = subsystem
        self.name = name
        self.additional_params: list[str] = []
        self.build_switch_cases_32bit: dict[int, list[str]] = {}
        self.build_switch_cases_64bit: dict[int, list[str]] = {}
        self.jmp_hook_targets: list[DllSubroutine] = []

    def generate(self, gen: CodeGenerator):
        func_name = f'MemoryPatch::{self.subsystem.name}::{self.name}'
        func_params = [f'void* p{self.subsystem.name}', 'int gameBuild'] + self.additional_params
        gen.begin_function('void', func_name, func_params)
        if self.jmp_hook_targets:
            self._generate_jmp_hook(gen)
        if len(self.build_switch_cases_32bit) > 1:
            # 32-bit Crysis Warhead is not supported
            for build in CRYSIS_WARHEAD_32BIT_BUILDS:
                assert build not in self.build_switch_cases_32bit
                self.build_switch_cases_32bit[build] = ['// TODO: 32-bit Crysis Warhead']
        gen.add('switch (gameBuild)')
        gen.begin_block()
        gen.add('#ifdef BUILD_64BIT', no_indent=True)
        gen.add_build_switch_cases(self.build_switch_cases_64bit)
        gen.add('#else', no_indent=True)
        gen.add_build_switch_cases(self.build_switch_cases_32bit)
        gen.add('#endif', no_indent=True)
        gen.end_block()
        gen.end_function()

    def _generate_jmp_hook(self, gen: CodeGenerator):
        gen.add('#ifdef BUILD_64BIT', no_indent=True)
        gen.add_assembly_as_c_array(self._get_jmp_hook_code(x64=True), 'code', const=False)
        gen.add('')
        gen.add('std::memcpy(&code[2], &handler, 8);')
        gen.add('#else', no_indent=True)
        gen.add_assembly_as_c_array(self._get_jmp_hook_code(x64=False), 'code', const=False)
        gen.add('')
        gen.add('std::memcpy(&code[1], &handler, 4);')
        gen.add('#endif', no_indent=True)
        gen.add('')
        for target in self.jmp_hook_targets:
            cases = self.build_switch_cases_64bit if target.dll.x64 else self.build_switch_cases_32bit
            build = target.dll.build
            if build not in cases:
                cases[build] = []
            cases[build].append(f'FillMem(p{self.subsystem.name}, 0x{target.subroutine.rva:x}, &code, sizeof(code));')

    def _get_jmp_hook_code(self, x64: bool) -> Assembly:
        assembly = Assembly(x64)
        if x64:
            assembly.add(Instruction.create_reg_u64(Code.MOV_R64_IMM64, Register.RAX, 0))
            assembly.add(Instruction.create_reg(Code.JMP_RM64, Register.RAX))
        else:
            assembly.add(Instruction.create_reg_u32(Code.MOV_R32_IMM32, Register.EAX, 0))
            assembly.add(Instruction.create_reg(Code.JMP_RM32, Register.EAX))
        # be nice to disassemblers and avoid any leftovers from the original instructions
        ideal_size = self._get_ideal_jmp_hook_size(required_size=assembly.code_size, x64=x64)
        # pad with nops if needed
        nop = Instruction.create(Code.NOPD)
        while assembly.code_size < ideal_size:
            assembly.add(nop)
        return assembly

    def _get_ideal_jmp_hook_size(self, required_size: int, x64: bool) -> int:
        ideal_size = 0
        for target in self.jmp_hook_targets:
            if target.dll.x64 != x64:
                continue
            size = 0
            for instruction in target.subroutine.instructions:
                assert len(instruction) != 0
                size += len(instruction)
                if size >= required_size:
                    break
            assert size >= required_size, 'Target subroutine is too small'
            if ideal_size == 0:
                ideal_size = size
            assert size == ideal_size, 'Target subroutine is not the same in all builds'
        return ideal_size

class HookGameWarning:
    '''Used to fix the GameWarning format string vulnerability.'''

    def _find_game_warning_32(self, dll: CryDll) -> Subroutine:
        assert not dll.x64
        found = []
        first_expected = Instruction.create_reg_i32(Code.MOV_R32_IMM32, Register.EAX, 0x1000)
        for first_index, first in enumerate(dll.disasm.instructions):
            if first != first_expected:
                continue
            sub = Subroutine.from_instruction(dll.disasm, first_index)
            if first_index != sub.begin_index or len(sub.instructions) < 14:
                continue
            if sub.instructions[1].code != Code.CALL_REL32_32:
                continue
            if sub.instructions[2].code != Code.MOV_R32_RM32 or sub.instructions[2].op0_register != Register.EAX:
                continue
            if sub.instructions[3] != Instruction.create_reg_reg(Code.TEST_RM32_R32, Register.EAX, Register.EAX):
                continue
            current = sub.instructions[4]
            if current.code != Code.JE_REL8_32 or current.near_branch_target != sub.instructions[-2].ip:
                continue
            found.append(sub)
        assert len(found) == 1
        return found[0]

    def _find_game_warning_64(self, dll: CryDll) -> Subroutine:
        assert dll.x64
        found = []
        first_expected = Instruction.create_reg_reg(Code.TEST_RM64_R64, Register.RCX, Register.RCX)
        for first_index, first in enumerate(dll.disasm.instructions):
            if first != first_expected:
                continue
            sub = Subroutine.from_instruction(dll.disasm, first_index)
            if first_index != sub.begin_index or len(sub.instructions) < 16:
                continue
            current = sub.instructions[1]
            if current.code != Code.JE_REL8_64 or current.near_branch_target != sub.instructions[-1].ip:
                continue
            if sub.instructions[2].code != Code.MOV_RM64_R64 or sub.instructions[2].op1_register != Register.RCX:
                continue
            if sub.instructions[3].code != Code.MOV_RM64_R64 or sub.instructions[3].op1_register != Register.RDX:
                continue
            if sub.instructions[4].code != Code.MOV_RM64_R64 or sub.instructions[4].op1_register != Register.R8:
                continue
            if sub.instructions[5].code != Code.MOV_RM64_R64 or sub.instructions[5].op1_register != Register.R9:
                continue
            if sub.instructions[6].code != Code.MOV_R32_IMM32 or sub.instructions[6].op0_register != Register.EAX:
                continue
            if sub.instructions[7].code != Code.CALL_REL32_64:
                continue
            found.append(sub)
        assert len(found) == 1
        return found[0]

    def generate(self, context: Context, gen: CodeGenerator):
        patch = MemoryPatch(context.subsystem, self.__class__.__name__)
        patch.additional_params.append('void (*handler)(const char* format, ...)')
        for dll in context.dlls:
            subroutine = self._find_game_warning_64(dll) if dll.x64 else self._find_game_warning_32(dll)
            patch.jmp_hook_targets.append(DllSubroutine(dll, subroutine))
        patch.generate(gen)

class HookCryWarning:
    '''Used to fix the CryWarning format string vulnerability.'''

    def _find_cry_warning_32(self, dll: CryDll) -> Subroutine:
        assert not dll.x64
        found = []
        first_expected = Instruction.create_reg_u32(Code.MOV_R32_IMM32, Register.EAX, 0x1000)
        for first_index, first in enumerate(dll.disasm.instructions):
            if first != first_expected:
                continue
            sub = Subroutine.from_instruction(dll.disasm, first_index)
            if first_index not in (sub.begin_index, sub.begin_index + 2) or len(sub.instructions) < 30:
                continue
            instructions = sub.instructions
            if first_index == sub.begin_index + 2:
                # ignore optional prologue
                instructions = instructions[2:]
            if instructions[1].code != Code.CALL_REL32_32:
                continue
            # skip index 2
            if instructions[3] != Instruction.create_reg_reg(Code.TEST_RM32_R32, Register.EAX, Register.EAX):
                continue
            if instructions[4].code != Code.JE_REL8_32 or instructions[4].near_branch_target != instructions[-2].ip:
                continue
            if instructions[5] != Instruction.create_mem_u32(Code.CMP_RM32_IMM8, MemoryOperand(Register.EAX), 0x0):
                continue
            if instructions[6].code != Code.JE_REL8_32 or instructions[6].near_branch_target != instructions[-2].ip:
                continue
            found.append(sub)
        assert len(found) == 1
        return found[0]

    def _find_cry_warning_64(self, dll: CryDll) -> Subroutine:
        assert dll.x64
        found = []
        expected = [
            Instruction.create_mem_reg(Code.MOV_RM64_R64,
                MemoryOperand(base=Register.RSP, displ=0x18, displ_size=0x1), Register.R8),
            Instruction.create_mem_reg(Code.MOV_RM64_R64,
                MemoryOperand(base=Register.RSP, displ=0x20, displ_size=0x1), Register.R9),
            Instruction.create_reg(Code.PUSH_R64, Register.RBX),
            Instruction.create_reg(Code.PUSH_R64, Register.RDI),
            Instruction.create_reg_u32(Code.MOV_R32_IMM32, Register.EAX, 0x1038),
        ]
        for first_index, first in enumerate(dll.disasm.instructions):
            if first != expected[0]:
                continue
            sub = Subroutine.from_instruction(dll.disasm, first_index)
            if first_index != sub.begin_index or len(sub.instructions) < 32:
                continue
            if expected[1:] != sub.instructions[1:5]:
                continue
            if sub.instructions[5].code != Code.CALL_REL32_64:
                continue
            if sub.instructions[6] != Instruction.create_reg_reg(Code.SUB_R64_RM64, Register.RSP, Register.RAX):
                continue
            found.append(sub)
        assert len(found) == 1
        return found[0]

    def generate(self, context: Context, gen: CodeGenerator):
        patch = MemoryPatch(context.subsystem, self.__class__.__name__)
        patch.additional_params.append('void (*handler)(int, int, const char* format, ...)')
        for dll in context.dlls:
            if dll.x64 and dll.build in CRYSIS_WARHEAD_BUILDS and context.subsystem is Subsystem.CryNetwork:
                patch.build_switch_cases_64bit[dll.build] = ['// Crysis Warhead has no CryWarning in its CryNetwork']
                continue
            subroutine = self._find_cry_warning_64(dll) if dll.x64 else self._find_cry_warning_32(dll)
            patch.jmp_hook_targets.append(DllSubroutine(dll, subroutine))
        patch.generate(gen)

PATCHES = {
    Subsystem.CryAction: [
        HookGameWarning(),
        HookCryWarning(),
    ],
    Subsystem.CryGame: [
        HookGameWarning(),
        HookCryWarning(),
    ],
    Subsystem.CryNetwork: [
        HookCryWarning(),
    ],
    Subsystem.CrySystem: [
        HookCryWarning(),
    ],
}

TARGET_PATH = Path(__file__).parent / '..' / 'Code' / 'Launcher' / 'MemoryPatch.cpp'
TARGET_MARKER = '// GENERATED MEMORY PATCHES'

class MemoryPatchGenerator:
    def __init__(self, full: bool):
        self.gen = CodeGenerator()
        self.full = full

    def run(self):
        self.gen.add('////////////////////////////////////////////////////////////////////////////////')
        self.gen.add(TARGET_MARKER)
        self.gen.add('////////////////////////////////////////////////////////////////////////////////')
        self._generate_patches(Subsystem.CryAction)
        self._generate_patches(Subsystem.CryGame)
        self._generate_patches(Subsystem.CryNetwork)
        self._generate_patches(Subsystem.CrySystem)
        if self.full:
            print('Writing...')
            lines = TARGET_PATH.read_text(encoding='utf-8').splitlines()
            lines = lines[:lines.index(TARGET_MARKER)]
            lines.pop()
            lines.extend(self.gen.lines)
            TARGET_PATH.write_text('\n'.join(lines) + '\n', encoding='utf-8')
        else:
            print('\n'.join(self.gen.lines))

    def _generate_patches(self, subsystem: Subsystem):
        self.gen.add('')
        self.gen.add('////////////////////////////////////////////////////////////////////////////////')
        self.gen.add(f'// {subsystem.name}')
        self.gen.add('////////////////////////////////////////////////////////////////////////////////')
        context = Context.load(subsystem, self.full)
        for patch in PATCHES[subsystem]:
            print(f'Generating {subsystem.name}::{patch.__class__.__name__}')
            self.gen.add('')
            self.gen.add_doc_comment(patch.__class__.__doc__)
            patch.generate(context, self.gen)

if __name__ == '__main__':
    MemoryPatchGenerator(full=True).run()
