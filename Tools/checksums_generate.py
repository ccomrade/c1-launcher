"""Generate checksums.py with all Crysis DLLs.

The following directory structure is used:
```
.
├── checksums_generate.py (this file)
├── checksums.py (generated)
└── DLLs
    ├── Crysis_5767
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_5879
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_6115
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_6156
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_Warhead_687
    │   └── Bin32
    │       └── *.dll
    ├── Crysis_Warhead_710
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       ├── *.dll
    │       └── Crysis64.exe
    ├── Crysis_Warhead_711
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       ├── *.dll
    │       └── Crysis64.exe
    ├── Crysis_Wars_6527
    │   └── Bin32
    │       └── *.dll
    ├── Crysis_Wars_6566
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_Wars_6586
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_Wars_6627
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    ├── Crysis_Wars_6670
    │   ├── Bin32
    │   │   └── *.dll
    │   └── Bin64
    │       └── *.dll
    └── Crysis_Wars_6729
        ├── Bin32
        │   └── *.dll
        └── Bin64
            └── *.dll
```
"""

from hashlib import sha256
from pathlib import Path
from typing import TextIO

def _scan(search_path: Path) -> dict[int, dict[int, dict[str, str]]]:
    """Return a dictionary with build -> bits -> name -> checksum."""
    data = {}
    for build_dir in search_path.glob('*/'):
        build = int(build_dir.name.split('_')[-1])
        assert build not in data
        data[build] = {}
        for bin_dir in build_dir.glob('*/'):
            bits = int(bin_dir.name.removeprefix('Bin'))
            assert bits not in data[build]
            data[build][bits] = {}
            for file in bin_dir.glob('*.*'):
                name = file.name
                checksum = sha256(file.read_bytes()).hexdigest()
                assert name not in data[build][bits]
                data[build][bits][name] = checksum
    return data

def _dump(data: dict[int, dict[int, dict[str, str]]], output: TextIO):
    output.write('"""Checksums of all Crysis DLLs."""\n')
    output.write('\n')
    output.write('CRY_CHECKSUMS = {\n')
    for build in sorted(data):
        output.write(f'    {build}' + ': {\n')
        for bits in sorted(data[build]):
            output.write(f'        {bits}' + ': {\n')
            for name in sorted(data[build][bits]):
                checksum = data[build][bits][name]
                output.write(f"            '{name}': '{checksum}',\n")
            output.write('        },\n')
        output.write('    },\n')
    output.write('}\n')

def _main():
    data = _scan(Path(__file__).with_name('DLLs'))
    with Path(__file__).with_name('checksums.py').open('w', encoding='utf-8') as output:
        _dump(data, output)

if __name__ == '__main__':
    _main()
