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

class BI_LZSS_Compressor : public StdXX::RedundancyBasedCompressor
{
	struct QueuedBlock
	{
		bool isRaw;
		uint8 rawByte;
		uint16 distance;
		uint16 length;
	};
public:
	//Constructor
	BI_LZSS_Compressor(StdXX::OutputStream &outputStream);

	//Properties
	inline uint32 CompressedSize() const
	{
		return this->compressedSize;
	}

	//Methods
	void Finalize() override;
	uint32 WriteBytes(const void *source, uint32 size) override;

private:
	//State
	uint8 currentBlockIndex;
	uint32 compressedSize;
	uint32 checksum;
	QueuedBlock queuedBlocks[8];

	//Methods
	int32 CheckPrimaryBackreference(uint16 distance, uint16 length) override;
	bool CheckSecondaryBackreference(uint16 d1, uint16 l1, uint16 d2, uint16 l2, uint16 distanceBetweenBackreferences) override;
	void EmitBackreference(uint16 distance, uint16 length) override;
	void EmitDirectByte() override;
	void PutBlockIntoQueue(const QueuedBlock& nextBlock);
	void WriteBatch();
};
