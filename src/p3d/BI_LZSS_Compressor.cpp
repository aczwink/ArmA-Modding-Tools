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
//Class header
#include "BI_LZSS_Compressor.hpp"
//Namespaces
using namespace StdXX;

static RedundancyBasedCompressor::Config BI_LZSS_Config()
{
	RedundancyBasedCompressor::Config c;

	c.initLength = 4096;
	c.initByte = 0x20;
	c.maxDistance = 4095;
	c.maxBackrefLength = 18;
	c.minBackrefLength = 3;

	return c;
}

//Constructor
BI_LZSS_Compressor::BI_LZSS_Compressor(OutputStream& outputStream) : RedundancyBasedCompressor(outputStream, BI_LZSS_Config())
{
	this->currentBlockIndex = 0;
	this->compressedSize = 0;
	this->checksum = 0;
}

//Public methods
void BI_LZSS_Compressor::Finalize()
{
	RedundancyBasedCompressor::Finalize();

	if(this->currentBlockIndex > 0)
		this->WriteBatch();

	//write check value
	DataWriter dataWriter(false, this->outputStream);
	dataWriter.WriteUInt32(this->checksum);
	this->compressedSize += 4;
}

uint32 BI_LZSS_Compressor::WriteBytes(const void *source, uint32 size)
{
	const uint8* src = static_cast<const uint8 *>(source);
	for(uint32 i = 0; i < size; i++)
		this->checksum += *src++;

	return RedundancyBasedCompressor::WriteBytes(source, size);
}

//Private methods
int32 BI_LZSS_Compressor::CheckPrimaryBackreference(uint16 distance, uint16 length)
{
	//in this format it never pays out to check for overlapping backreferences
	return 0;
}

bool BI_LZSS_Compressor::CheckSecondaryBackreference(uint16 d1, uint16 l1, uint16 d2, uint16 l2, uint16 distanceBetweenBackreferences)
{
	//won't ever happen
	return false;
}

void BI_LZSS_Compressor::EmitBackreference(uint16 distance, uint16 length)
{
	QueuedBlock block;
	block.isRaw = false;
	block.distance = distance;
	block.length = length;

	this->PutBlockIntoQueue(block);
}

void BI_LZSS_Compressor::EmitDirectByte()
{
	QueuedBlock block;
	block.isRaw = true;
	this->ReadSlidingWindowPart(&block.rawByte, 0, 1);

	this->PutBlockIntoQueue(block);
}

void BI_LZSS_Compressor::PutBlockIntoQueue(const QueuedBlock& nextBlock)
{
	this->queuedBlocks[this->currentBlockIndex++] = nextBlock;

	if(this->currentBlockIndex == 8)
		this->WriteBatch();
}

void BI_LZSS_Compressor::WriteBatch()
{
	uint8 flagByte = 0;
	for(uint8 i = 0; i < this->currentBlockIndex; i++)
	{
		flagByte >>= 1;
		if(this->queuedBlocks[i].isRaw)
			flagByte |= 0x80;
	}
	flagByte >>= (8 - this->currentBlockIndex);

	DataWriter dataWriter(false, this->outputStream);

	dataWriter.WriteByte(flagByte);
	this->compressedSize++;

	for(uint8 i = 0; i < this->currentBlockIndex; i++)
	{
		const auto& block = this->queuedBlocks[i];
		if(block.isRaw)
		{
			dataWriter.WriteByte(block.rawByte);
			this->compressedSize++;
		}
		else
		{
			uint8 b1 = static_cast<uint8>(block.distance & 0xFF_u8);
			uint8 b2 = static_cast<uint8>(((block.distance >> 4_u8) & 0xF0) | (block.length - 3_u16));

			dataWriter.WriteByte(b1);
			dataWriter.WriteByte(b2);

			this->compressedSize += 2;
		}
	}

	this->currentBlockIndex = 0;
}
