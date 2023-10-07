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
	free(this->lodData.pVertices);
	free(this->lodData.pNormals);
	for(uint32 i = 0; i < this->nTags; i++)
	{
		free(this->lodData.pTags[i].pData);
	}
	free(this->lodData.pTags);
}

//Public methods
uint32 MLOD_SP3X_Lod::GetNumberOfPolygons() const
{
	return this->lodData.polygons.GetNumberOfElements();
}

void MLOD_SP3X_Lod::GetPolygon(uint32 index, P3DPolygon &polygon) const
{
	polygon.texturePath = this->lodData.polygons[index].texturePath;
}

LodType MLOD_SP3X_Lod::GetType() const
{
	return LodType::MLOD_SP3X;
}

void MLOD_SP3X_Lod::Write(OutputStream &outputStream) const
{
	uint32 size;

	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	textWriter.WriteString(P3D_LODHEADER_SP3X_SIGNATURE);
	dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMAJOR);
	dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMINOR);

	dataWriter.WriteUInt32(this->lodData.nVertices);
	dataWriter.WriteUInt32(this->lodData.nNormals);
	dataWriter.WriteUInt32(this->lodData.polygons.GetNumberOfElements());
	dataWriter.WriteUInt32(this->lodData.unknownFlags);

	size = this->lodData.nVertices * sizeof(*this->lodData.pVertices);
	dataWriter.WriteBytes(this->lodData.pVertices, size);

	size = this->lodData.nNormals * sizeof(*this->lodData.pNormals);
	dataWriter.WriteBytes(this->lodData.pNormals, size);

	for(const auto& polygon : this->lodData.polygons)
	{
		if(polygon.texturePath.GetSize() > P3D_MLOD_SP3X_PATHLENGTH)
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
		textWriter.WriteFixedLengthString(polygon.texturePath, P3D_MLOD_SP3X_PATHLENGTH);
		dataWriter.WriteUInt32(polygon.type);

		for (const auto& vertexTable : polygon.vertexTables)
		{
			dataWriter.WriteUInt32(vertexTable.verticesIndex);
			dataWriter.WriteUInt32(vertexTable.normalsIndex);
			dataWriter.WriteFloat32(vertexTable.u);
			dataWriter.WriteFloat32(vertexTable.v);
		}

		dataWriter.WriteUInt32(polygon.flags);
	}

	dataWriter.WriteBytes(&this->lodData.tagSignature, sizeof(this->lodData.tagSignature));
	this->WriteTags(this->lodData.pTags, outputStream);

	dataWriter.WriteFloat32(this->lodData.resolution);
}

//Private methods
void MLOD_SP3X_Lod::Read(InputStream &inputStream)
{
	uint32 size;

	DataReader dataReader(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	this->lodData.nVertices = dataReader.ReadUInt32();
	this->lodData.nNormals = dataReader.ReadUInt32();
	uint32 nPolygons = dataReader.ReadUInt32();
	this->lodData.unknownFlags = dataReader.ReadUInt32();

	size = this->lodData.nVertices * sizeof(*this->lodData.pVertices);
	this->lodData.pVertices = (SVertex *)malloc(size);
	dataReader.ReadBytes(this->lodData.pVertices, size);

	size = this->lodData.nNormals * sizeof(*this->lodData.pNormals);
	this->lodData.pNormals = (Math::Vector3S *)malloc(size);
	dataReader.ReadBytes(this->lodData.pNormals, size);

	this->lodData.polygons.Resize(nPolygons);
	for(uint32 i = 0; i < nPolygons; i++)
	{
		auto& polygon = this->lodData.polygons[i];

		polygon.texturePath = textReader.ReadZeroTerminatedStringBySize(P3D_MLOD_SP3X_PATHLENGTH);
		polygon.type = dataReader.ReadUInt32();

		for (auto& vertexTable : polygon.vertexTables)
		{
			vertexTable.verticesIndex = dataReader.ReadUInt32();
			vertexTable.normalsIndex = dataReader.ReadUInt32();
			vertexTable.u = dataReader.ReadFloat32();
			vertexTable.v = dataReader.ReadFloat32();
		}

		polygon.flags = dataReader.ReadUInt32();
	}

	dataReader.ReadBytes(&this->lodData.tagSignature, sizeof(this->lodData.tagSignature));
	this->ReadTags(dataReader);

	this->lodData.resolution = dataReader.ReadFloat32();
}

void MLOD_SP3X_Lod::ReadTags(DataReader& dataReader)
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

	this->lodData.pTags = (SMLODTag *)malloc(tags.GetNumberOfElements() * sizeof(*this->lodData.pTags));
	for(uint32 i = 0; i < tags.GetNumberOfElements(); i++)
	{
		this->lodData.pTags[i] = tags[i];
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