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
#include <libBISMod/p3d/MLOD_P3DM.hpp>
//Local
#include <libBISMod/p3d/MLOD.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;

//Destructor
MLOD_P3DM_Lod::~MLOD_P3DM_Lod()
{
	free(this->lodData.pVertices);
	free(this->lodData.pNormals);

	for(uint32 i = 0; i < this->lodData.nPolygons; i++)
	{
		free(this->lodData.pPolygons[i].pTextureName);
		free(this->lodData.pPolygons[i].pMaterialName);
	}
	free(this->lodData.pPolygons);

	for(uint32 i = 0; i < this->nTags; i++)
	{
		free(this->lodData.pTags[i].pTagName);
		free(this->lodData.pTags[i].pData);
	}
	free(this->lodData.pTags);
}

//Public methods
void MLOD_P3DM_Lod::Write(OutputStream &outputStream) const
{
	uint32 size;

	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	textWriter.WriteString(P3D_LODHEADER_P3DM_SIGNATURE);
	dataWriter.WriteUInt32(P3D_LODHEADER_P3DM_VERSIONMAJOR);
	dataWriter.WriteUInt32(P3D_LODHEADER_P3DM_VERSIONMINOR);

	dataWriter.WriteUInt32(this->lodData.nVertices);
	dataWriter.WriteUInt32(this->lodData.nNormals);
	dataWriter.WriteUInt32(this->lodData.nPolygons);
	dataWriter.WriteUInt32(this->lodData.unknownFlags);

	size = this->lodData.nVertices * sizeof(*this->lodData.pVertices);
	dataWriter.WriteBytes(this->lodData.pVertices, size);

	size = this->lodData.nNormals * sizeof(*this->lodData.pNormals);
	dataWriter.WriteBytes(this->lodData.pNormals, size);

	for(uint32 i = 0; i < this->lodData.nPolygons; i++)
	{
		dataWriter.WriteUInt32(this->lodData.pPolygons[i].type);
		dataWriter.WriteBytes(&this->lodData.pPolygons[i].vertexTables, sizeof(this->lodData.pPolygons[i].vertexTables));
		dataWriter.WriteUInt32(this->lodData.pPolygons[i].flags);
		textWriter.WriteString(this->lodData.pPolygons[i].pTextureName);
		dataWriter.WriteByte(0);
		textWriter.WriteString(this->lodData.pPolygons[i].pMaterialName);
		dataWriter.WriteByte(0);
	}

	dataWriter.WriteBytes(&this->lodData.tagSignature, sizeof(this->lodData.tagSignature));

	for(uint32 i = 0; i < this->nTags; i++)
	{
		dataWriter.WriteByte(this->lodData.pTags[i].active);
		textWriter.WriteString(this->lodData.pTags[i].pTagName);
		dataWriter.WriteByte(0);
		dataWriter.WriteUInt32(this->lodData.pTags[i].dataSize);
		dataWriter.WriteBytes(this->lodData.pTags[i].pData, this->lodData.pTags[i].dataSize);
	}

	dataWriter.WriteFloat32(this->lodData.resolution);
}

//Private methods
void MLOD_P3DM_Lod::Read(InputStream &inputStream)
{
	String buffer;
	uint32 size;

	DataReader dataReader(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	this->lodData.nVertices = dataReader.ReadUInt32();
	this->lodData.nNormals = dataReader.ReadUInt32();
	this->lodData.nPolygons = dataReader.ReadUInt32();
	this->lodData.unknownFlags = dataReader.ReadUInt32();

	size = this->lodData.nVertices * sizeof(*this->lodData.pVertices);
	this->lodData.pVertices = (SVertex *)malloc(size);
	dataReader.ReadBytes(this->lodData.pVertices, size);

	size = this->lodData.nNormals * sizeof(*this->lodData.pNormals);
	this->lodData.pNormals = (Math::Vector3S *)malloc(size);
	dataReader.ReadBytes(this->lodData.pNormals, size);

	this->lodData.pPolygons = (SP3DMPolygon *)malloc(this->lodData.nPolygons * sizeof(*this->lodData.pPolygons));
	for(uint32 i = 0; i < this->lodData.nPolygons; i++)
	{
		this->lodData.pPolygons[i].type = dataReader.ReadUInt32();
		dataReader.ReadBytes(&this->lodData.pPolygons[i].vertexTables, sizeof(this->lodData.pPolygons[i].vertexTables));
		this->lodData.pPolygons[i].flags = dataReader.ReadUInt32();
		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.pPolygons[i].pTextureName = (char *)malloc(buffer.GetLength()+1);
		MemCopy(this->lodData.pPolygons[i].pTextureName, buffer.GetRawZeroTerminatedData(), buffer.GetLength());
		this->lodData.pPolygons[i].pTextureName[buffer.GetLength()] = '\0';
		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.pPolygons[i].pMaterialName = (char *)malloc(buffer.GetLength()+1);
		MemCopy(this->lodData.pPolygons[i].pMaterialName, buffer.GetRawZeroTerminatedData(), buffer.GetLength());
		this->lodData.pPolygons[i].pMaterialName[buffer.GetLength()] = '\0';
	}

	dataReader.ReadBytes(&this->lodData.tagSignature, sizeof(this->lodData.tagSignature));
	this->ReadTags(inputStream);

	this->lodData.resolution = dataReader.ReadFloat32();
}

void MLOD_P3DM_Lod::ReadTags(InputStream &inputStream)
{
	DynamicArray<SP3DMTag> tags;
	SP3DMTag tmp;
	String buffer;

	DataReader refFile(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	while(true)
	{
		MemZero(&tmp, sizeof(tmp));

		tmp.active = refFile.ReadByte() != 0;
		buffer = textReader.ReadZeroTerminatedString();
		tmp.pTagName = (char *)malloc(buffer.GetLength()+1);
		MemCopy(tmp.pTagName, buffer.GetRawZeroTerminatedData(), buffer.GetLength());
		tmp.pTagName[buffer.GetLength()] = '\0';
		tmp.dataSize = refFile.ReadUInt32();
		tmp.pData = (byte *)malloc(tmp.dataSize);
		refFile.ReadBytes(tmp.pData, tmp.dataSize);

		tags.Push(tmp);

		if(tmp.pTagName == (String)P3D_TAGGS_ENDOFFILE)
			break;
	}

	this->lodData.pTags = (SP3DMTag *)malloc(tags.GetNumberOfElements() * sizeof(*this->lodData.pTags));
	for(uint32 i = 0; i < tags.GetNumberOfElements(); i++)
	{
		this->lodData.pTags[i] = tags[i];
	}
	this->nTags = tags.GetNumberOfElements();
}
