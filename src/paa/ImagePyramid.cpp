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
#include <StdXXStreams.hpp>
//Class header
#include <libBISMod/paa/ImagePyramid.hpp>
#include <Std++/Multimedia/PixelFormat.hpp>
#include "../p3d/LZSS.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::Multimedia;

//Constructor
ImagePyramid::ImagePyramid(SeekableInputStream &inputStream)
{
	DataReader dataReader(false, inputStream);

	this->codecType = static_cast<PAACodecType>(dataReader.ReadUInt16());
	this->ReadTags(inputStream);

	uint16 nPaletteEntries = dataReader.ReadUInt16();
	ASSERT_EQUALS(0, nPaletteEntries);

	BufferedInputStream bufferedInputStream(inputStream);
	DataReader bufferedDataReader(false, bufferedInputStream);
	this->ReadMipMaps(bufferedInputStream);

	ASSERT_EQUALS(0, bufferedDataReader.ReadUInt16());
	ASSERT(bufferedInputStream.IsAtEnd(), u8"TODO: do this correctly");
}

//Public methods
CodingFormatId ImagePyramid::QueryCodingFormatId() const
{
	switch(codecType)
	{
		case PAACodecType::ABGR4444:
			return CodingFormatId::RawVideo;
		case PAACodecType::S3TC_DXT1:
			return CodingFormatId::S3TC_DXT1;
	}

	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

StdXX::Optional<StdXX::Multimedia::PixelFormat> ImagePyramid::QueryPixelFormat() const
{
	switch(codecType)
	{
		case PAACodecType::ABGR4444:
		{
			auto fmt = Multimedia::PixelFormat(Multimedia::ColorSpace::RGBA);

			for(uint8 i = 0; i < 4; i++)
			{
				fmt.colorComponents[i].planeIndex = 0;
				fmt.colorComponents[i].isFloat = false;
				fmt.colorComponents[i].max.u8 = 15;
				fmt.colorComponents[i].min.u8 = 0;
				fmt.colorComponents[i].nBits = 4;
			}
			fmt.colorComponents[0].shift = 0;
			fmt.colorComponents[1].shift = 4;
			fmt.colorComponents[2].shift = 8;
			fmt.colorComponents[3].shift = 12;

			fmt.nPlanes = 1;
			fmt.planes[0].horzSampleFactor = 1;
			fmt.planes[0].vertSampleFactor = 1;

			return fmt;
		}
	}

	return {};
}

//Private methods
uint32 ImagePyramid::ComputeImageByteSize(uint16 width, uint16 height) const
{
	switch(this->codecType)
	{
		case PAACodecType::ABGR4444:
			return width * height * 2_u32;
		case PAACodecType::S3TC_DXT1:
			return (width * height) / 2_u32;
	}
	NOT_IMPLEMENTED_ERROR; //TODO: implement me
}

void ImagePyramid::ReadMipMaps(InputStream &inputStream)
{
	DataReader dataReader(false, inputStream);

	MipMap mipmap;
	while(true)
	{
		mipmap.width = dataReader.ReadUInt16();
		mipmap.height = dataReader.ReadUInt16();

		if (mipmap.width && mipmap.height)
		{
			uint8 sizeRaw[3];
			dataReader.ReadBytes(sizeRaw, sizeof(sizeRaw));

			uint32 blockSize = sizeRaw[0] | (sizeRaw[1] << 8) | (sizeRaw[2] << 16);
			uint32 realSize = this->ComputeImageByteSize(mipmap.width, mipmap.height);

			mipmap.data.Resize(realSize);
			if(blockSize == realSize)
				dataReader.ReadBytes(mipmap.data.Data(), blockSize);
			else
			{
				FixedSizeBuffer blockBuffer(blockSize);
				dataReader.ReadBytes(blockBuffer.Data(), blockSize);

				BufferInputStream bufferInputStream(blockBuffer.Data(), blockSize);

				LZSSReadCompressed(mipmap.data.Data(), realSize, bufferInputStream);
			}

			this->mipmaps.Push(Move(mipmap));
		}
		else
			break;
	}
}

void ImagePyramid::ReadTags(SeekableInputStream &inputStream)
{
	DataReader dataReader(false, inputStream);
	DataReader fourccReader(true, inputStream); //all fourCCs are reversed in PAAs

	while(true)
	{
		uint32 tagSignature = fourccReader.ReadUInt32();
		if(tagSignature != FOURCC(u8"TAGG"))
		{
			inputStream.SeekTo(inputStream.QueryCurrentOffset() - 4);
			break;
		}

		uint32 tagName = fourccReader.ReadUInt32();
		uint32 dataLength = dataReader.ReadUInt32();

		switch(tagName)
		{
			case FOURCC(u8"AVGC"): //average color
				ASSERT_EQUALS(4, dataLength);
				dataReader.Skip(dataLength);
				break;
			case FOURCC(u8"FLAG"):
				ASSERT_EQUALS(4, dataLength);
				dataReader.Skip(dataLength);
				break;
			case FOURCC(u8"OFFS"):
				ASSERT_EQUALS(16*4, dataLength);
				dataReader.Skip(dataLength);
				break;
			default:
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
	}
}