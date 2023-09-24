/*
 * Copyright (c) 2023 Amir Czwink (amir130@hotmail.de)
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
#include <StdXXMultimedia.hpp>
//Local
#include "WSSCompressedCodingFormat.hpp"
#include "WSSDecoder.hpp"

class WSSDemuxer : public StdXX::Multimedia::Demuxer
{
public:
	//Constructor
	inline WSSDemuxer(const StdXX::Multimedia::Format& format, StdXX::SeekableInputStream& inputStream) : Demuxer(format, inputStream)
	{
		this->wssCompressedCodingFormat.AddDecoder(new WSSDecoder, 1.0f);
	}

	//Methods
	void ReadHeader() override;
	void Seek(uint64 timestamp, const class StdXX::Multimedia::TimeScale &timeScale) override;

private:
	//Methods
	WSSCompressedCodingFormat wssCompressedCodingFormat;
	StdXX::UniquePointer<StdXX::Multimedia::IPacket> ReadPacket() override;
};