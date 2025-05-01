from iced_x86 import Decoder, Code, Instruction
from pefile import PE, MACHINE_TYPE

def get_bits(pe: PE) -> int:
    if pe.FILE_HEADER.Machine == MACHINE_TYPE['IMAGE_FILE_MACHINE_I386']:
        return 32
    if pe.FILE_HEADER.Machine == MACHINE_TYPE['IMAGE_FILE_MACHINE_AMD64']:
        return 64
    assert False, 'Only x86 is supported'

def get_text_section_data_and_ip(pe: PE) -> tuple[bytes, int]:
    pe_base = pe.OPTIONAL_HEADER.ImageBase
    for section in pe.sections:
        if section.Name == b'.text\x00\x00\x00':
            return section.get_data(ignore_padding=True), pe_base + section.VirtualAddress
    assert False, 'Cannot find .text section'

def disassemble(pe: PE) -> list[Instruction]:
    text_data, text_ip = get_text_section_data_and_ip(pe)
    return list(Decoder(bitness=get_bits(pe), data=text_data, ip=text_ip))

class XRefs:
    def __init__(self):
        self.rvas: dict[int, list[int]] = {}
        self.count = 0

def find_xrefs(pe: PE, instructions: list[Instruction]) -> XRefs:
    xrefs = XRefs()
    pe_base = pe.OPTIONAL_HEADER.ImageBase
    pe_end = pe_base + pe.OPTIONAL_HEADER.SizeOfImage
    for index, instruction in enumerate(instructions):
        displacement = instruction.memory_displacement
        if pe_base <= displacement < pe_end:
            rva = displacement - pe_base
            if rva not in xrefs.rvas:
                xrefs.rvas[rva] = []
            xrefs.rvas[rva].append(index)
            xrefs.count += 1
    return xrefs

class Disassembler:
    def __init__(self, pe: PE):
        self.pe = pe
        self.pe_base = int(pe.OPTIONAL_HEADER.ImageBase)
        self.pe_end = int(self.pe_base + pe.OPTIONAL_HEADER.SizeOfImage)
        self.x64 = bool(pe.FILE_HEADER.Machine == MACHINE_TYPE['IMAGE_FILE_MACHINE_AMD64'])
        self.instructions = disassemble(pe)
        self.xrefs = find_xrefs(pe, self.instructions)

    def dump_xrefs(self):
        for rva, indexes in self.xrefs.rvas.items():
            for index in indexes:
                print(f'{self.instruction_to_string(index)} -> 0x{rva:x}')

    def instruction_to_rva(self, index: int) -> int:
        return self.instructions[index].ip - self.pe_base

    def instruction_to_string(self, index: int) -> str:
        instruction = self.instructions[index]
        return f'{instruction.ip:016X} {instruction:XrsSMi}'

    def has_call_xref(self, index: int) -> bool:
        rva = self.instruction_to_rva(index)
        if rva not in self.xrefs.rvas:
            return False
        call_code = Code.CALL_REL32_64 if self.x64 else Code.CALL_REL32_32
        for x in self.xrefs.rvas[rva]:
            instruction = self.instructions[x]
            if instruction.code == call_code:
                return True
        return False

    def has_ret_before(self, index: int) -> bool:
        if index == 0:
            return False
        ret_code = Code.RETNQ if self.x64 else Code.RETND
        return self.instructions[index - 1].code == ret_code

    def has_call_after(self, index: int) -> bool:
        if (index + 1) >= len(self.instructions):
            return False
        call_code = Code.CALL_REL32_64 if self.x64 else Code.CALL_REL32_32
        return self.instructions[index + 1].code == call_code

class Subroutine:
    def __init__(self, disasm: Disassembler, begin_index: int, end_index: int):
        self.disasm = disasm
        self.begin_index = begin_index
        self.end_index = end_index

    @classmethod
    def from_instruction(cls, disasm: Disassembler, index: int):
        begin_index = index
        while begin_index > 0:
            if disasm.has_call_xref(begin_index):
                break
            prev_index = begin_index - 1
            prev_instruction = disasm.instructions[prev_index]
            if prev_instruction.code == Code.INT3:
                break
            begin_index -= 1
        end_index = index + 1
        while end_index < len(disasm.instructions):
            if disasm.has_call_xref(end_index):
                break
            next_instruction = disasm.instructions[end_index]
            if next_instruction.code == Code.INT3:
                break
            end_index += 1
        return cls(disasm, begin_index, end_index)

    @property
    def rva(self) -> int:
        return self.disasm.instruction_to_rva(self.begin_index)

    @property
    def instructions(self) -> list[Instruction]:
        return self.disasm.instructions[self.begin_index:self.end_index]

    def dump(self):
        for index in range(self.begin_index, self.end_index):
            print(self.disasm.instruction_to_string(index))
