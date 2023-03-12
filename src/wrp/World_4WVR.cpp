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
//Class header
#include "World_4WVR.hpp"
//Libs
#include <StdXXStreams.hpp>
//Local
#include "../Shared.hpp"
//Namespaces
using namespace StdXX;

//Constructor
World_4WVR::World_4WVR(uint32 dimX, uint32 dimY, SeekableInputStream &inputStream) : elevations(dimY, dimX), textureIndices(dimY, dimX)
{
	DataReader dataReader(false, inputStream);

	for(uint32 y = 0; y < dimY; y++)
	{
		for(uint32 x = 0; x < dimX; x++)
		{
			this->elevations(y, x) = dataReader.ReadUInt16();
		}
	}

	for(uint32 y = 0; y < dimY; y++)
	{
		for(uint32 x = 0; x < dimX; x++)
		{
			this->textureIndices(y, x) = dataReader.ReadUInt16();
		}
	}

	TextReader textReader(inputStream, TextCodecType::ASCII);

	for(uint32 i = 0; i < WRP_4WVR_NUMBEROFTEXTUREFILENAMES; i++)
	{
		this->textureFileNames[i] = textReader.ReadZeroTerminatedString(WRP_4WVR_TEXTUREFILENAMELENGTH);
	}

	while(!inputStream.IsAtEnd())
	{
		Object* pObject = new Object;

		pObject->transformation = ReadTransformation(inputStream);
		pObject->id = dataReader.ReadUInt32();
		pObject->p3dFileName = textReader.ReadZeroTerminatedString(WRP_4WVR_OBJECT_P3DFILENAMELENGTH);

		this->objects.Push(pObject);
	}
}

//Public methods
void World_4WVR::Write(OutputStream &outputStream) const
{
	uint32 dimX = this->elevations.GetNumberOfColumns();
	uint32 dimY = this->elevations.GetNumberOfRows();

	DataWriter dataWriter(false, outputStream);

	dataWriter.WriteBytes(WRP_4WVR_SIGNATURE, WRP_4WVR_SIGNATURELENGTH);
	dataWriter.WriteUInt32(dimX);
	dataWriter.WriteUInt32(dimY);

	for(uint32 y = 0; y < dimY; y++)
	{
		for(uint32 x = 0; x < dimX; x++)
		{
			dataWriter.WriteUInt16(this->elevations(y, x));
		}
	}

	for(uint32 y = 0; y < dimY; y++)
	{
		for(uint32 x = 0; x < dimX; x++)
		{
			dataWriter.WriteUInt16(this->textureIndices(y, x));
		}
	}

	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	for(uint32 i = 0; i < WRP_4WVR_NUMBEROFTEXTUREFILENAMES; i++)
	{
		textWriter.WriteFixedLengthString(this->textureFileNames[i], WRP_4WVR_TEXTUREFILENAMELENGTH);
	}

	for(const auto& object : this->objects)
	{
		WriteTransformation(object->transformation, dataWriter);
		dataWriter.WriteUInt32(object->id);
		textWriter.WriteFixedLengthString(object->p3dFileName, WRP_4WVR_OBJECT_P3DFILENAMELENGTH);
	}
}
