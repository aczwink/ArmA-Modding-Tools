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
#include <libBISMod/p3d/MLOD_Lod.hpp>
//Local
#include <libBISMod/p3d/MLOD.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;
//Definitions
#define P3D_LODHEADER_P3DM_SIGNATURE u8"P3DM"
#define P3D_LODHEADER_P3DM_VERSIONMAJOR 0x1C
#define P3D_LODHEADER_P3DM_VERSIONMINOR 0x100
#define P3D_LODHEADER_SP3X_SIGNATURE "SP3X"
#define P3D_LODHEADER_SP3X_VERSIONMAJOR 0x1C
#define P3D_LODHEADER_SP3X_VERSIONMINOR 0x99
#define P3D_MLOD_TAGS_SP3X_NAME_LENGTH 64
#define P3D_TAGS_ENDOFFILE u8"#EndOfFile#"
#define P3D_TAGS_SIGNATURE u8"TAGG"
#define P3D_TAGS_SIGNATURE_LENGTH 4

//Public methods
uint32 MLOD_Lod::GetNumberOfPolygons() const
{
	return this->faces.GetNumberOfElements();
}

void MLOD_Lod::GetPolygon(uint32 index, P3DPolygon &polygon) const
{
	polygon.texturePath = this->faces[index].texturePath;
}

LodType MLOD_Lod::GetType() const
{
	return LodType::MLOD_SP3X;
}

void MLOD_Lod::Write(OutputStream &outputStream) const
{
	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	if(this->isSP3X)
	{
		textWriter.WriteString(P3D_LODHEADER_SP3X_SIGNATURE);
		dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMAJOR);
		dataWriter.WriteUInt32(P3D_LODHEADER_SP3X_VERSIONMINOR);
	}
	else
	{
		textWriter.WriteString(P3D_LODHEADER_P3DM_SIGNATURE);
		dataWriter.WriteUInt32(P3D_LODHEADER_P3DM_VERSIONMAJOR);
		dataWriter.WriteUInt32(P3D_LODHEADER_P3DM_VERSIONMINOR);
	}

	dataWriter.WriteUInt32(this->vertices.GetNumberOfElements());
	dataWriter.WriteUInt32(this->faceNormals.GetNumberOfElements());
	dataWriter.WriteUInt32(this->faces.GetNumberOfElements());
	dataWriter.WriteUInt32(this->unknownFlags);

	for(const auto& vertex : this->vertices)
	{
		this->WriteVec3(vertex.pos, dataWriter);
		dataWriter.WriteUInt32(vertex.flags);
	}

	for(const auto& normal : this->faceNormals)
		this->WriteVec3(normal, dataWriter);

	for(const auto& polygon : this->faces)
	{
		if(this->isSP3X)
			textWriter.WriteFixedLengthString(polygon.texturePath, P3D_MLOD_SP3X_PATHLENGTH);

		dataWriter.WriteUInt32(static_cast<uint32>(polygon.type));

		for (const auto& vertexTable : polygon.vertexTables)
		{
			dataWriter.WriteUInt32(vertexTable.vertexIndex);
			dataWriter.WriteUInt32(vertexTable.normalIndex);
			dataWriter.WriteFloat32(vertexTable.u);
			dataWriter.WriteFloat32(vertexTable.v);
		}

		dataWriter.WriteUInt32(polygon.flags);

		if(!this->isSP3X)
		{
			textWriter.WriteStringZeroTerminated(polygon.texturePath);
			textWriter.WriteStringZeroTerminated(polygon.materialName);
		}
	}

	dataWriter.WriteBytes(P3D_TAGS_SIGNATURE, P3D_TAGS_SIGNATURE_LENGTH);
	for(const auto& tag : this->tags)
		this->WriteTag(tag, dataWriter);
	this->WriteTag({true, P3D_TAGS_ENDOFFILE}, dataWriter);

	dataWriter.WriteFloat32(this->resolution);
}

//Private methods
void MLOD_Lod::Read(InputStream &inputStream)
{
	DataReader dataReader(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	MLOD_LodHeader header;

	dataReader.ReadBytes(&header.signature, sizeof(header.signature));
	header.versionMajor = dataReader.ReadUInt32();
	header.versionMinor = dataReader.ReadUInt32();

	if(String::CopyUtf8Bytes(header.signature, sizeof(header.signature)) == P3D_LODHEADER_SP3X_SIGNATURE)
		this->isSP3X = true;
	else if(String::CopyUtf8Bytes(header.signature, sizeof(header.signature)) == P3D_LODHEADER_P3DM_SIGNATURE)
		this->isSP3X = false;
	else
		NOT_IMPLEMENTED_ERROR; //TODO: implement me

	uint32 nVertices = dataReader.ReadUInt32();
	uint32 nNormals = dataReader.ReadUInt32();
	uint32 nPolygons = dataReader.ReadUInt32();
	this->unknownFlags = dataReader.ReadUInt32();

	this->vertices.Resize(nVertices);
	for(uint32 i = 0; i < nVertices; i++)
	{
		auto& v = this->vertices[i];

		this->ReadVec3(v.pos, dataReader);
		v.flags = dataReader.ReadUInt32();
	}

	this->faceNormals.Resize(nNormals);
	for(uint32 i = 0; i < nNormals; i++)
		this->ReadVec3(this->faceNormals[i], dataReader);

	this->faces.Resize(nPolygons);
	for(uint32 i = 0; i < nPolygons; i++)
	{
		auto& polygon = this->faces[i];

		if(this->isSP3X)
			polygon.texturePath = textReader.ReadZeroTerminatedStringBySize(P3D_MLOD_SP3X_PATHLENGTH);

		polygon.type = static_cast<MLOD_FaceType>(dataReader.ReadUInt32());

		for (auto& vertexTable : polygon.vertexTables)
		{
			vertexTable.vertexIndex = dataReader.ReadUInt32();
			vertexTable.normalIndex = dataReader.ReadUInt32();
			vertexTable.u = dataReader.ReadFloat32();
			vertexTable.v = dataReader.ReadFloat32();
		}

		polygon.flags = dataReader.ReadUInt32();

		if(!this->isSP3X)
		{
			polygon.texturePath = textReader.ReadZeroTerminatedString();
			polygon.materialName = textReader.ReadZeroTerminatedString();
		}
	}

	String signature = textReader.ReadStringBySize(P3D_TAGS_SIGNATURE_LENGTH);
	ASSERT_EQUALS(P3D_TAGS_SIGNATURE, signature);
	this->ReadTags(inputStream);

	this->resolution = dataReader.ReadFloat32();
}

void MLOD_Lod::ReadTags(InputStream& inputStream)
{
	DataReader dataReader(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	while(true)
	{
		MLOD_Tag tmp;

		if(!this->isSP3X)
			tmp.active = dataReader.ReadByte() != 0;

		if(this->isSP3X)
			tmp.name = textReader.ReadZeroTerminatedStringBySize(P3D_MLOD_TAGS_SP3X_NAME_LENGTH);
		else
			tmp.name = textReader.ReadZeroTerminatedString();

		uint32 dataSize = dataReader.ReadUInt32();
		tmp.payload.Resize(dataSize);
		dataReader.ReadBytes(tmp.payload.Data(), dataSize);

		if(tmp.name == P3D_TAGS_ENDOFFILE)
			break;

		this->tags.Push(Move(tmp));
	}
}

void MLOD_Lod::WriteTag(const MLOD_Tag& tag, DataWriter& dataWriter) const
{
	TextWriter textWriter(dataWriter.Stream(), TextCodecType::ASCII);

	if(!this->isSP3X)
		dataWriter.WriteByte(static_cast<byte>(tag.active));

	if(this->isSP3X)
		textWriter.WriteFixedLengthString(tag.name, P3D_MLOD_TAGS_SP3X_NAME_LENGTH);
	else
		textWriter.WriteStringZeroTerminated(tag.name);

	dataWriter.WriteUInt32(tag.payload.Size());
	dataWriter.WriteBytes(tag.payload.Data(), tag.payload.Size());
}