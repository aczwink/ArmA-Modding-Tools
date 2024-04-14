/*
 * Copyright (c) 2023-2024 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of ArmA-Modding-Tools.
 *
 * ArmA-Modding-Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArmA-Modding-Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArmA-Modding-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <StdXX.hpp>
//Corresponding header
#include "LZSS.hpp"
#include "BI_LZSS_Compressor.hpp"
//Definitions
#define LZSS_SPACEFILTER 0x20

//Local functions
static uint32 CalculateChecksum(byte *pUncompressedData, uint32 size, bool isSigned = false)
{
	uint32 checksum = 0;
	byte *pEnd = &pUncompressedData[size];

	if(isSigned)
	{
		while(pUncompressedData < pEnd) checksum += (char)*pUncompressedData++;
	}
	else
	{
		while(pUncompressedData < pEnd) checksum += *pUncompressedData++;
	}

	return checksum;
}

//Global functions
void LZSSCompressData(const byte *pInput, uint32 uncompressedSize, byte *pOutput, uint32 &refOutputSize)
{
	BufferInputStream inputStream(pInput, uncompressedSize);
	BufferOutputStream outputStream(pOutput, 2 * uncompressedSize);

	BI_LZSS_Compressor compressor(outputStream);
	inputStream.FlushTo(compressor);
	compressor.Finalize();

	refOutputSize = compressor.CompressedSize();
}

bool LZSSReadCompressed(byte *pOutput, uint32 uncompressedSize, InputStream &refInput)
{
	byte flagByte, bits, rlen;
	uint32 outputOffset = 0, rpos, readChecksum;
	byte b1, b2; //IF ERRORS... TRY THOSE CHARS
	byte *pFrom, *pTo;

	DataReader dataReader(false, refInput);

	while(uncompressedSize)
	{
		flagByte = dataReader.ReadByte();
		for(bits = 0; bits < 8; bits++, flagByte >>= 1)
		{
			if(flagByte & 1)
			{
				pOutput[outputOffset++] = dataReader.ReadByte();

				if(!--uncompressedSize) goto finish;
			}
			else
			{
				b1 = dataReader.ReadByte();
				b2 = dataReader.ReadByte();
				rpos = b1 + ((b2 & 0xF0) << 4);
				rlen = (b2 & 0xF) + 3;

				if(!rpos) return false;

				while(rpos > outputOffset)
				{
					pOutput[outputOffset++] = LZSS_SPACEFILTER;

					if(!--uncompressedSize) goto finish;
					if(!--rlen) goto nextBit;
				}

				rpos = outputOffset - rpos;
				pFrom = &pOutput[rpos];
				pTo = &pOutput[outputOffset];
				outputOffset += rlen;

				while(rlen--)
				{
					*pTo++ = *pFrom++;
					if(!--uncompressedSize) goto finish;
				}
			}
			nextBit:;
		}
	}

	finish:
	readChecksum = dataReader.ReadUInt32();
	if(readChecksum != CalculateChecksum(pOutput, outputOffset))
	{
		if(readChecksum != CalculateChecksum(pOutput, outputOffset, true))
		{
			return false;
		}
	}

	return true;
}