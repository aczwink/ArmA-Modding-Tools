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
#include <StdXXCore.hpp>
#include <StdXXMultimedia.hpp>

namespace libBISMod
{
	class WSSFormat : public StdXX::Multimedia::Format
	{
	public:
		//Methods
		StdXX::Multimedia::Demuxer *CreateDemuxer(StdXX::SeekableInputStream &inputStream) const override;
		StdXX::Multimedia::Muxer *CreateMuxer(StdXX::SeekableOutputStream &outputStream) const override;
		StdXX::String GetExtension() const override;
		void GetFormatInfo(StdXX::Multimedia::FormatInfo &refFormatInfo) const override;
		StdXX::String GetName() const override;
		StdXX::DynamicArray<const StdXX::Multimedia::CodingFormat *> GetSupportedCodingFormats(StdXX::Multimedia::DataType dataType) const override;
		float32 Probe(StdXX::BufferInputStream &refBuffer) const override;
	};
}