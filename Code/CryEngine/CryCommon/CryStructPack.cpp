// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
// 
#include <CryStructPack.h>

#if NEED_STRUCT_PACK
uint32 StructSize(const CTypeInfo& typeInfo, uint32 limit)
{
	uint32 size = 0;

	for AllSubVars (pVar, typeInfo)
	{
		if (pVar->Offset >= limit)
			break;
		if (pVar->Type.IsType<void*>())
			size += 4 * pVar->ArrayDim;
		else if (pVar->Type.HasSubVars())
		{
			uint32 typeSize = StructSize(pVar->Type);
			if (pVar->ArrayDim * typeSize > limit - pVar->Offset)
				size = limit;
			else
				size += pVar->ArrayDim * typeSize;
		} else
			size += pVar->GetSize();
	}
	return size;
}

uint32 StructUnpack(const CTypeInfo& typeInfo, uint8* ptr, const uint8* buffer,
										uint32 limit, bool expandPointers)
{
	uint32 offset = 0;
	for AllSubVars (pVar, typeInfo)
	{
		if (pVar->Offset >= limit)
			break;
		if (pVar->Type.IsType<void*>())
		{
			if (expandPointers)
			{
				for (int j = 0; j < pVar->ArrayDim; ++j)
				{
					INT_PTR v = buffer[offset] | (buffer[offset + 1] << 8)
						| (buffer[offset + 2] << 16) | (buffer[offset + 3] << 24);
					memcpy(
						(uint8*)ptr + pVar->Offset + j * sizeof(void*),
						&v,
						sizeof(void*));
					offset += 4;
				}
			} else
			{
				memset((uint8*)ptr + pVar->Offset, 0, pVar->GetSize());
				offset += 4 * pVar->ArrayDim;
			}
		} else if (pVar->Type.HasSubVars())
		{
			uint32 varOffset = pVar->Offset;
			for (int j = 0; j < pVar->ArrayDim; ++j)
			{
				offset += StructUnpack(pVar->Type, ptr + varOffset, buffer + offset,
					limit - varOffset, expandPointers);
				varOffset += pVar->Type.Size;
				if (varOffset >= limit) break;
			}
		} else
		{
			uint32 varSize = pVar->GetSize();
			memcpy((uint8*)ptr + pVar->Offset, buffer + offset, varSize);
#ifdef NEED_ENDIAN_SWAP
			SwapEndian((uint8*)ptr + pVar->Offset,
				pVar->ArrayDim,	pVar->Type, pVar->Type.Size);
#endif
			offset += varSize;
		}
	}
	return offset;
}

uint32 StructPack(const CTypeInfo& typeInfo, const uint8* ptr, uint8* buffer,
									uint32 limit, bool packPointers)
{
	uint32 offset = 0;
	for AllSubVars (pVar, typeInfo)
	{
		if (pVar->Offset >= limit)
			break;
		if (pVar->Type.IsType<void*>())
		{
			if (packPointers)
			{
				// Note that only the least significant 32 bits of the pointer are
				// packed for storage.
				for (int i = 0; i < pVar->ArrayDim; ++i)
				{
					INT_PTR v = 0;
					memcpy(
						&v,
						(const uint8*)ptr + pVar->Offset + i * sizeof(void*),
						sizeof(void*));
					buffer[offset] = (uint8)(v & 0xff);
					buffer[offset + 1] = (uint8)((v >> 8) & 0xff);
					buffer[offset + 2] = (uint8)((v >> 16) & 0xff);
					buffer[offset + 3] = (uint8)((v >> 24) & 0xff);
					offset += 4;
				}
			} else
			{
				memset(buffer + offset, 0, 4 * pVar->ArrayDim);
				offset += 4 * pVar->ArrayDim;
			}
		} else if (pVar->Type.HasSubVars())
		{
			uint32 varOffset = pVar->Offset;
			for (int j = 0; j < pVar->ArrayDim; ++j)
			{
				offset += StructPack(pVar->Type, ptr + varOffset, buffer + offset,
					limit - varOffset, packPointers);
				varOffset += pVar->Type.Size;
				if (varOffset >= limit) break;
			}
		} else
		{
			uint32 varSize = pVar->GetSize();
			memcpy(buffer + offset, (const uint8*)ptr + pVar->Offset, varSize);
#ifdef NEED_ENDIAN_SWAP
			SwapEndian(buffer + offset,
				pVar->ArrayDim, pVar->Type, pVar->Type.Size);
#endif
			offset += varSize;
		}
	}
	return offset;
}
#endif
