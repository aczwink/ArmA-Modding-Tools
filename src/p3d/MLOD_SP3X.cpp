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
#include <libBISMod/p3d/MLOD_SP3X.hpp>
//Local
#include <libBISMod/p3d/MLOD.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;

//Destructor
MLOD_SP3X_Lod::~MLOD_SP3X_Lod()
{
	free(this->lod.pVertices);
	free(this->lod.pNormals);
	free(this->lod.pPolygons);
	for(uint32 i = 0; i < this->nTags; i++)
	{
		free(this->lod.pTags[i].pData);
	}
	free(this->lod.pTags);
}

//Public methods
void MLOD_SP3X_Lod::Write(OutputStream &outputStream) const
{
	uint32 size;

	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	textWriter.WriteString(P3D_LODHEADER_SP3X_SIGNATURE);
	dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMAJOR);
	dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMINOR);

	dataWriter.WriteUInt32(this->lod.nVertices);
	dataWriter.WriteUInt32(this->lod.nNormals);
	dataWriter.WriteUInt32(this->lod.nPolygons);
	dataWriter.WriteUInt32(this->lod.unknownFlags);

	size = this->lod.nVertices * sizeof(*this->lod.pVertices);
	dataWriter.WriteBytes(this->lod.pVertices, size);

	size = this->lod.nNormals * sizeof(*this->lod.pNormals);
	dataWriter.WriteBytes(this->lod.pNormals, size);

	size = this->lod.nPolygons * sizeof(*this->lod.pPolygons);
	dataWriter.WriteBytes(this->lod.pPolygons, size);

	dataWriter.WriteBytes(&this->lod.tagSignature, sizeof(this->lod.tagSignature));
	this->WriteTags(this->lod.pTags, outputStream);

	dataWriter.WriteFloat32(this->lod.resolution);
}

//Private methods
void MLOD_SP3X_Lod::Read(InputStream &inputStream)
{
	uint32 size;

	DataReader dataReader(false, inputStream);

	this->lod.nVertices = dataReader.ReadUInt32();
	this->lod.nNormals = dataReader.ReadUInt32();
	this->lod.nPolygons = dataReader.ReadUInt32();
	this->lod.unknownFlags = dataReader.ReadUInt32();

	size = this->lod.nVertices * sizeof(*this->lod.pVertices);
	this->lod.pVertices = (SVertex *)malloc(size);
	dataReader.ReadBytes(this->lod.pVertices, size);

	size = this->lod.nNormals * sizeof(*this->lod.pNormals);
	this->lod.pNormals = (Math::Vector3S *)malloc(size);
	dataReader.ReadBytes(this->lod.pNormals, size);

	size = this->lod.nPolygons * sizeof(*this->lod.pPolygons);
	this->lod.pPolygons = (SSP3XPolygon *)malloc(size);
	dataReader.ReadBytes(this->lod.pPolygons, size);

	dataReader.ReadBytes(&this->lod.tagSignature, sizeof(this->lod.tagSignature));
	this->ReadTags(this->lod.pTags, dataReader);

	this->lod.resolution = dataReader.ReadFloat32();
}

void MLOD_SP3X_Lod::ReadTags(SMLODTag *pTags, DataReader& dataReader)
{
	DynamicArray<SMLODTag> tags;
	SMLODTag tmp;

	while(true)
	{
		MemZero(&tmp, sizeof(tmp));
		dataReader.ReadBytes(&tmp.name, sizeof(tmp.name));
		tmp.dataSize = dataReader.ReadUInt32();
		tmp.pData = (byte *)malloc(tmp.dataSize);
		dataReader.ReadBytes(tmp.pData, tmp.dataSize);

		tags.Push(tmp);

		if(tmp.name == (String)P3D_TAGGS_ENDOFFILE)
		{
			break;
		}
	}

	pTags = (SMLODTag *)malloc(tags.GetNumberOfElements() * sizeof(*pTags));
	for(uint32 i = 0; i < tags.GetNumberOfElements(); i++)
	{
		pTags[i] = tags[i];
	}
	this->nTags = tags.GetNumberOfElements();
}

void MLOD_SP3X_Lod::WriteTags(SMLODTag *pTags, OutputStream &outputStream) const
{
	DataWriter dataWriter(false, outputStream);

	for(uint32 i = 0; i < this->nTags; i++)
	{
		dataWriter.WriteBytes(pTags[i].name, sizeof(pTags[i].name));
		dataWriter.WriteUInt32(pTags[i].dataSize);
		dataWriter.WriteBytes(pTags[i].pData, pTags[i].dataSize);
	}
}