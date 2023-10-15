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
//Corresponding header
#include <libBISMod/p3d/P3D.hpp>
//Local
#include <libBISMod/p3d/ODOL7Lod.hpp>
#include <libBISMod/p3d/ODOL7ModelInfo.hpp>
#include <libBISMod/p3d/MLOD.hpp>
#include <libBISMod/p3d/MLOD_Lod.hpp>
#include <libBISMod/p3d/MLODModelInfo.hpp>
#include "Definitions.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
//Definitions
#define P3D_HEADER_VERSION_MLOD 0x101

struct P3DHeader
{
	P3DType type;
	uint32 version;
	uint32 lodCount;
};

//Local functions
static bool ReadP3DHeader(P3DHeader& header, InputStream& inputStream)
{
	uint8 signature[4];

	DataReader dataReader(false, inputStream);

	dataReader.ReadBytes(signature, sizeof(signature));
	header.version = dataReader.ReadUInt32();
	header.lodCount = dataReader.ReadUInt32();

	if(String::CopyUtf8Bytes(signature, sizeof(signature)) == P3D_HEADER_SIGNATURE_MLOD)
		header.type = P3DType::MLOD;
	else if(String::CopyUtf8Bytes(signature, sizeof(signature)) == P3D_HEADER_SIGNATURE_ODOL)
	{
		switch(header.version)
		{
			case P3D_HEADER_VERSION_ODOL7:
				header.type = P3DType::ODOL7;
				break;
			default:
				return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static UniquePointer<P3DLod> ReadLod(P3DType type, InputStream& inputStream)
{
	switch(type)
	{
		case P3DType::MLOD:
			return new MLOD_Lod(inputStream);
		case P3DType::ODOL7:
			return new ODOL7Lod(inputStream);
	}
}

static UniquePointer<P3DModelInfo> ReadModelInfo(P3DType type, InputStream& inputStream, uint32 nLods)
{
	switch(type)
	{
		case P3DType::MLOD:
			return new MLODModelInfo(inputStream);
		case P3DType::ODOL7:
			return new ODOL7ModelInfo(inputStream, nLods);
	}
}

//Public methods
UniquePointer<P3DData> P3DData::Convert(LodType targetLodType) const
{
	P3DType targetType = this->MapLodTypeToP3DType(targetLodType);
	UniquePointer<P3DData> mapped = new P3DData(targetType);

	for(uint32 i = 0; i < this->lods.GetNumberOfElements(); i++)
	{
		auto mappedLod = this->ConvertLod(i, targetLodType);
		mapped->lods.Push(Move(mappedLod));
	}

	if(targetLodType == LodType::ODOL7)
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	else
		mapped->modelInfo = new MLODModelInfo;

	return mapped;
}

void P3DData::Write(OutputStream& outputStream) const
{
	DataWriter dataWriter(false, outputStream);

	switch(this->Type())
	{
		case P3DType::MLOD:
		{
			dataWriter.WriteBytes(P3D_HEADER_SIGNATURE_MLOD, 4);
			dataWriter.WriteUInt32(P3D_HEADER_VERSION_MLOD);
		}
			break;
		case P3DType::ODOL7:
			dataWriter.WriteBytes(P3D_HEADER_SIGNATURE_ODOL, 4);
			dataWriter.WriteUInt32(P3D_HEADER_VERSION_ODOL7);
			break;
	}
	dataWriter.WriteUInt32(this->lods.GetNumberOfElements());

	for(const auto& lod : this->lods)
		lod->Write(outputStream);

	this->modelInfo->Write(outputStream);
}

//Private methods
UniquePointer<P3DLod> P3DData::ConvertLod(uint32 lodIndex, LodType targetType) const
{
	switch(this->type)
	{
		case P3DType::ODOL7:
		{
			switch(targetType)
			{
				case LodType::MLOD_SP3X:
				case LodType::MLOD_P3DM:
					return this->ConvertODOL7LodToMLOD(*this->lods[lodIndex], targetType, lodIndex);
			}
		}
		break;
	}

	NOT_IMPLEMENTED_ERROR; //TODO: implement me
	return UniquePointer<P3DLod>();
}

StdXX::UniquePointer<P3DLod> P3DData::ConvertODOL7LodToMLOD(const P3DLod& sourceLod, LodType targetType, uint32 lodIndex) const
{
	const auto& src = dynamic_cast<const ODOL7Lod &>(sourceLod).lodData;
	const ODOL7ModelInfo& odol7ModelInfo = dynamic_cast<const ODOL7ModelInfo&>(*this->modelInfo);

	UniquePointer<MLOD_Lod> sp3xLod = new MLOD_Lod(targetType == LodType::MLOD_SP3X);
	auto& dest = *sp3xLod;

	dest.unknownFlags = 0;
	dest.vertices.Resize(src.vertexTable.nVertices);
	dest.faceNormals.Resize(src.vertexTable.nNormals);
	dest.faces.Resize(src.faces.nFaces);

	for(uint32 i = 0; i < src.vertexTable.nVertices; i++)
	{
		auto& v = dest.vertices[i];
		v.pos = src.vertexTable.pVertices[i] + odol7ModelInfo.modelInfo.centreOfGravity;
		v.flags = src.vertexTable.pVerticesFlags[i];
	}

	for(uint32 i = 0; i < src.vertexTable.nNormals; i++)
	{
		dest.faceNormals[i] = src.vertexTable.pNormals[i];
	}

	for(uint32 i = 0; i < src.faces.nFaces; i++)
	{
		auto& destFace = dest.faces[i];
		const auto& srcFace = src.faces.pPolygons[i];

		destFace.type = static_cast<MLOD_FaceType>(srcFace.type);
		for(uint8 j = 0; j < (uint8)destFace.type; j++)
		{
			//TODO: MLOD and ODOL have reversed vertex order (one is clockwise, the other one is counter-clock wise). Document the order for each. In addition, compared to mlod, odol starts with the second vertex
			uint8 verticesPerFace = (uint8)destFace.type;
			uint8 odol2MlodVertexTableIndex = ( ((verticesPerFace - 1_u8) - j) + (verticesPerFace - 2_u8) ) % verticesPerFace;

			uint16 vertexTableIndex = srcFace.pVertexIndices[odol2MlodVertexTableIndex];

			destFace.vertexTables[j].normalIndex = vertexTableIndex;
			destFace.vertexTables[j].vertexIndex = vertexTableIndex;
			destFace.vertexTables[j].u = src.vertexTable.pUVPairs[vertexTableIndex].e[0];
			destFace.vertexTables[j].v = src.vertexTable.pUVPairs[vertexTableIndex].e[1];
		}
		if(destFace.type == MLOD_FaceType::Triangle)
		{
			MemZero(&destFace.vertexTables[3], sizeof(destFace.vertexTables));
		}

		destFace.flags = srcFace.flags;
		destFace.texturePath = src.textures[srcFace.textureIndex];
	}

	for(uint32 i = 0; i < src.namedSelections.nSelections; i++)
	{
		const auto& srcSelection = src.namedSelections.pSelections[i];

		MLOD_Tag tag;
		tag.name = srcSelection.name;
		tag.payload.Resize(dest.vertices.GetNumberOfElements() + dest.faces.GetNumberOfElements());

		MemZero(tag.payload.Data(), tag.payload.Size());

		for(uint32 j = 0; j < srcSelection.nVertexTableIndices; j++)
			tag.payload[srcSelection.pVertexTableIndices[j]] = 1;

		for(uint32 j = 0; j < srcSelection.nPolygons; j++)
			tag.payload[dest.vertices.GetNumberOfElements() + srcSelection.pPolygonIndices[j]] = 1;

		dest.tags.Push(Move(tag));
	}

	dest.resolution = odol7ModelInfo.modelInfo.resolutions[lodIndex];

	return sp3xLod;
}

P3DType P3DData::MapLodTypeToP3DType(LodType lodType) const
{
	switch(lodType)
	{
		case LodType::MLOD_SP3X:
		case LodType::MLOD_P3DM:
			return P3DType::MLOD;
		case LodType::ODOL7:
			return P3DType::ODOL7;
	}
}

//Namespace functions
String libBISMod::LodResolutionToString(float resolution)
{
#define CMP(right, retstring) if(Float<float64>::AlmostEqual(resolution, right, Float<float64>::MachineEpsilon())){return retstring;}
	CMP(LOD_RESOLUTION_VIEWGUNNER, "View - Gunner");
	CMP(LOD_RESOLUTION_VIEWPILOT, "View - Pilot");
	CMP(LOD_RESOLUTION_VIEWCARGO, "View - Cargo");
	CMP(LOD_RESOLUTION_GEOMETRY, "Geometry");
	CMP(LOD_RESOLUTION_MEMORY, "Memory");
	CMP(LOD_RESOLUTION_LANDCONTACT, "LandContact");
	CMP(LOD_RESOLUTION_ROADWAY, "RoadWay");
	CMP(LOD_RESOLUTION_PATHS, "Paths");
	CMP(LOD_RESOLUTION_HITPOINTS, "Hit-points");
	CMP(LOD_RESOLUTION_VIEWGEOMETRY, "View Geometry");
	CMP(LOD_RESOLUTION_FIREGEOMETRY, "Fire Geometry");
	CMP(LOD_RESOLUTION_VIEWCARGOGEOMETRY, "View - Cargo - Geometry");
	CMP(LOD_RESOLUTION_VIEWCARGOFIREGEOMETRY, "View - Cargo - Fire Geometry");
	CMP(LOD_RESOLUTION_VIEWCOMMANDER, "View - Commander");
	CMP(LOD_RESOLUTION_VIEWCOMMANDERGEOMETRY, "View - Commander - Geometry");
	CMP(LOD_RESOLUTION_VIEWCOMMANDERFIREGEOMETRY, "View - Commander - Fire Geometry");
	CMP(LOD_RESOLUTION_VIEWPILOTGEOMETRY, "View - Pilot - Geometry");
	CMP(LOD_RESOLUTION_VIEWPILOTFIREGEOMETRY, "View - Pilot - Fire Geometry");
	CMP(LOD_RESOLUTION_VIEWGUNNERGEOMETRY, "View - Gunner - Geometry");
	CMP(LOD_RESOLUTION_VIEWGUNNERFIREGEOMETRY, "View - Gunner - Fire Geometry");

	return String::Number(resolution, FloatDisplayMode::FixedPointNotation, 3);
}

String libBISMod::P3dTypeToString(P3DType type)
{
	switch(type)
	{
		case P3DType::MLOD:
			return "MLOD";
		case P3DType::ODOL7:
			return "ODOL v7";
	}

	return "illegal";
}

UniquePointer<P3DData> libBISMod::ReadP3DFile(InputStream& inputStream)
{
	P3DHeader header;
	if(ReadP3DHeader(header, inputStream))
	{
		UniquePointer<P3DData> data = new P3DData(header.type);

		for(uint32 i = 0; i < header.lodCount; i++)
		{
			UniquePointer<P3DLod> lod = ReadLod(header.type, inputStream);

			data->lods.Push(Move(lod));
		}
		data->modelInfo = ReadModelInfo(header.type, inputStream, header.lodCount);

		return data;
	}

	return nullptr;
}