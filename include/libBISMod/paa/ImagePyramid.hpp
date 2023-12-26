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
#pragma once
#include <StdXXCore.hpp>
#include <StdXXMultimedia.hpp>

namespace libBISMod
{
	enum class PAACodecType
	{
		ABGR4444 = 0x4444,
		GrayWithAlpha = 0x8080,
		S3TC_DXT1 = 0xFF01,
	};

	struct MipMap
	{
		uint16 width;
		uint16 height;
		StdXX::DynamicByteBuffer data;
	};

	class ImagePyramid
	{
	public:
		//Constructor
		ImagePyramid(StdXX::SeekableInputStream& inputStream);

		//Methods
		StdXX::Multimedia::CodingFormatId QueryCodingFormatId() const;
		StdXX::Optional<StdXX::Multimedia::PixelFormat> QueryPixelFormat() const;

		//Inline
		inline const MipMap& GetMipMap(uint8 level) const
		{
			return this->mipmaps[level];
		}

	private:
		//State
		PAACodecType codecType;
		StdXX::DynamicArray<MipMap> mipmaps;

		//Methods
		uint32 ComputeImageByteSize(uint16 width, uint16 height) const;
		void ReadMipMaps(StdXX::InputStream& inputStream);
		void ReadTags(StdXX::SeekableInputStream& inputStream);
	};
}