#include <intrin.h>  // __cpuid

#include "CPU.h"

/**
 * @brief Checks if AMD processor is being used.
 * @return True if we are running on AMD processor, otherwise false.
 */
bool CPU::IsAMD()
{
	const char *vendorID = "AuthenticAMD";  // must be 12 characters long

	int registers[4];
	__cpuid(registers, 0x0);

	const int *id = reinterpret_cast<const int*>(vendorID);

	return registers[1] == id[0]   // 1st part is in EBX register
	    && registers[3] == id[1]   // 2nd part is in EDX register
	    && registers[2] == id[2];  // 3rd part is in ECX register
}

/**
 * @brief Checks if the processor supports 3DNow! instructions.
 * @return True if 3DNow! instruction set is available, otherwise false.
 */
bool CPU::Has3DNow()
{
	int registers[4];
	__cpuid(registers, 0x80000001);

	int bit3DNow = registers[3] & (1 << 31);  // bit 31 in EDX register

	return bit3DNow != 0;
}
