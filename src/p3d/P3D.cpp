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
#include <libBISMod/p3d/MLOD_SP3X.hpp>
#include <libBISMod/p3d/MLOD_P3DM.hpp>
#include "Definitions.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;

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
		{
			SMLODLodHeader header;

			DataReader dataReader(false, inputStream);

			dataReader.ReadBytes(&header.signature, sizeof(header.signature));
			header.versionMajor = dataReader.ReadUInt32();
			header.versionMinor = dataReader.ReadUInt32();

			if(String::CopyUtf8Bytes(header.signature, sizeof(header.signature)) == P3D_LODHEADER_SP3X_SIGNATURE)
			{
				return new MLOD_SP3X_Lod(inputStream);
			}
			else if(String::CopyUtf8Bytes(header.signature, sizeof(header.signature)) == P3D_LODHEADER_P3DM_SIGNATURE)
			{
				return new MLOD_P3DM_Lod(inputStream);
			}
			else
				NOT_IMPLEMENTED_ERROR; //TODO: implement me
		}
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

void libBISMod::WriteP3DFile(const FileSystem::Path& path, const P3DData& data)
{
	FileOutputStream fileOutputStream(path);
	DataWriter dataWriter(false, fileOutputStream);

	switch(data.Type())
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
	dataWriter.WriteUInt32(data.lods.GetNumberOfElements());

	for(const auto& lod : data.lods)
		lod->Write(fileOutputStream);

	data.modelInfo->Write(fileOutputStream);
}