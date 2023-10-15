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
#include <StdXX.hpp>
//Class header
#include <libBISMod/p3d/ODOL7ModelInfo.hpp>
//Local
#include <libBISMod/p3d/MLODModelInfo.hpp>
#include <libBISMod/p3d/P3D.hpp>
#include "LZSS.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;

//Destructor
ODOL7ModelInfo::~ODOL7ModelInfo()
{
	free(this->modelInfo.pUnknownFloats);
}

//Public methods
void ODOL7ModelInfo::Write(OutputStream &outputStream) const
{
	byte *pCompressedData;
	uint32 compressedSize, uncompressedSize;

	DataWriter dataWriter(false, outputStream);

	for(float32 resolution : this->modelInfo.resolutions)
		dataWriter.WriteFloat32(resolution);

	dataWriter.WriteUInt32(this->modelInfo.index);
	dataWriter.WriteFloat32(this->modelInfo.memLodSphere);
	dataWriter.WriteFloat32(this->modelInfo.geoLodSphere);
	dataWriter.WriteBytes(&this->modelInfo.vertexFlags, sizeof(this->modelInfo.vertexFlags));
	dataWriter.WriteBytes(&this->modelInfo.offset1, sizeof(this->modelInfo.offset1));
	dataWriter.WriteUInt32(this->modelInfo.mapIconColor);
	dataWriter.WriteUInt32(this->modelInfo.mapSelectedColor);
	dataWriter.WriteFloat32(this->modelInfo.viewDensity);
	dataWriter.WriteBytes(&this->modelInfo.bBoxMinPosition, sizeof(this->modelInfo.bBoxMinPosition));
	dataWriter.WriteBytes(&this->modelInfo.bBoxMaxPosition, sizeof(this->modelInfo.bBoxMaxPosition));
	dataWriter.WriteBytes(&this->modelInfo.centreOfGravity, sizeof(this->modelInfo.centreOfGravity));
	dataWriter.WriteBytes(&this->modelInfo.offset2, sizeof(this->modelInfo.offset2));
	dataWriter.WriteBytes(&this->modelInfo.cogOffset, sizeof(this->modelInfo.cogOffset));
	dataWriter.WriteBytes(&this->modelInfo.modelMassVectors, sizeof(this->modelInfo.modelMassVectors));
	dataWriter.WriteByte(this->modelInfo.autoCenter);
	dataWriter.WriteByte(this->modelInfo.lockAutoCenter);
	dataWriter.WriteByte(this->modelInfo.canOcclude);
	dataWriter.WriteByte(this->modelInfo.canBeOccluded);
	dataWriter.WriteByte(this->modelInfo.allowAnimation);
	dataWriter.WriteByte(this->modelInfo.unknown);

	dataWriter.WriteUInt32(this->modelInfo.nUknownFloats);
	uncompressedSize = this->modelInfo.nUknownFloats * sizeof(*this->modelInfo.pUnknownFloats);

	if(uncompressedSize > P3D_GAPSIZE_COMPRESSION)
	{
		pCompressedData = (byte *)malloc(uncompressedSize * 2); //lame because compression does not work correctly
		LZSSCompressData((byte *)this->modelInfo.pUnknownFloats, uncompressedSize, pCompressedData, compressedSize);
		dataWriter.WriteBytes(pCompressedData, compressedSize);
		free(pCompressedData);
	}
	else
	{
		dataWriter.WriteBytes(this->modelInfo.pUnknownFloats, uncompressedSize);
	}

	dataWriter.WriteFloat32(this->modelInfo.mass);
	dataWriter.WriteFloat32(this->modelInfo.massReciprocal);
	dataWriter.WriteFloat32(this->modelInfo.altMass);
	dataWriter.WriteFloat32(this->modelInfo.altMassReciprocal);
	dataWriter.WriteBytes(&this->modelInfo.unknownByteIndices, sizeof(this->modelInfo.unknownByteIndices));
}


//Private methods
void ODOL7ModelInfo::Read(InputStream &inputStream, uint32 nLods)
{
	DataReader dataReader(false, inputStream);

	this->modelInfo.resolutions.Resize(nLods);
	for(uint32 i = 0; i < nLods; i++)
	{
		this->modelInfo.resolutions[i] = dataReader.ReadFloat32();
	}
	this->modelInfo.index = dataReader.ReadUInt32();
	this->modelInfo.memLodSphere = dataReader.ReadFloat32();
	this->modelInfo.geoLodSphere = dataReader.ReadFloat32();
	this->modelInfo.vertexFlags[0] = dataReader.ReadUInt32();
	this->modelInfo.vertexFlags[1] = dataReader.ReadUInt32();
	this->modelInfo.vertexFlags[2] = dataReader.ReadUInt32();
	dataReader.ReadBytes(&this->modelInfo.offset1, sizeof(this->modelInfo.offset1));
	this->modelInfo.mapIconColor = dataReader.ReadUInt32();
	this->modelInfo.mapSelectedColor = dataReader.ReadUInt32();
	this->modelInfo.viewDensity = dataReader.ReadFloat32();
	dataReader.ReadBytes(&this->modelInfo.bBoxMinPosition, sizeof(this->modelInfo.bBoxMinPosition));
	dataReader.ReadBytes(&this->modelInfo.bBoxMaxPosition, sizeof(this->modelInfo.bBoxMaxPosition));
	dataReader.ReadBytes(&this->modelInfo.centreOfGravity, sizeof(this->modelInfo.centreOfGravity));
	dataReader.ReadBytes(&this->modelInfo.offset2, sizeof(this->modelInfo.offset2));
	dataReader.ReadBytes(&this->modelInfo.cogOffset, sizeof(this->modelInfo.cogOffset));
	dataReader.ReadBytes(&this->modelInfo.modelMassVectors, sizeof(this->modelInfo.modelMassVectors));
	this->modelInfo.autoCenter = this->ReadBool(dataReader);
	this->modelInfo.lockAutoCenter = this->ReadBool(dataReader);
	this->modelInfo.canOcclude = this->ReadBool(dataReader);
	this->modelInfo.canBeOccluded = this->ReadBool(dataReader);
	this->modelInfo.allowAnimation = this->ReadBool(dataReader);
	this->modelInfo.unknown = dataReader.ReadByte();
	this->modelInfo.nUknownFloats = dataReader.ReadUInt32();
	uint32 size = this->modelInfo.nUknownFloats * sizeof(*this->modelInfo.pUnknownFloats);
	this->modelInfo.pUnknownFloats = (float *)malloc(size);
	if(size > P3D_GAPSIZE_COMPRESSION)
	{
		LZSSReadCompressed((byte *)this->modelInfo.pUnknownFloats, size, inputStream);
	}
	else
	{
		dataReader.ReadBytes(this->modelInfo.pUnknownFloats, size);
	}
	this->modelInfo.mass = dataReader.ReadFloat32();
	this->modelInfo.massReciprocal = dataReader.ReadFloat32();
	this->modelInfo.altMass = dataReader.ReadFloat32();
	this->modelInfo.altMassReciprocal = dataReader.ReadFloat32();
	dataReader.ReadBytes(&this->modelInfo.unknownByteIndices, sizeof(this->modelInfo.unknownByteIndices));
}
