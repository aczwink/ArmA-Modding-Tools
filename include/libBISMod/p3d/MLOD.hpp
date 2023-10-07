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
//Definitions
#define P3D_HEADER_VERSION_MLOD 0x101

#define P3D_TAGGS_ENDOFFILE "#EndOfFile#"

#define VERTEX_FLAG_NORMAL							0
#define VERTEX_FLAG_SURFACE_ONSURFACE				0x1
#define VERTEX_FLAG_SURFACE_ABOVESURFACE			0x2
#define VERTEX_FLAG_SURFACE_UNDERSURFACE			0x4
#define VERTEX_FLAG_SURFACE_KEEPHEIGHT				0x8
#define VERTEX_FLAG_LIGHTNING_SHINING				0x10
#define VERTEX_FLAG_LIGHTNING_ALWAYSINSHADOW		0x20
#define VERTEX_FLAG_LIGHTNING_FULLYLIGHTED			0x40
#define VERTEX_FLAG_LIGHTNING_HALFLIGHTED			0x80
#define VERTEX_FLAG_DECAL							0x100
#define VERTEX_FLAG_DECAL_RADIO12					0x200
#define VERTEX_FLAG_FOG_NONE						0x1000
#define VERTEX_FLAG_FOG_SKY							0x2000
#define VERTEX_FLAG_VISIBLE							0
#define VERTEX_FLAG_HIDDEN							0x1000000

#define POLYGON_FLAG_NONE							0
#define POLYGON_FLAG_DISABLESHADOW					0x10
#define POLYGON_FLAG_ZBIAS_LOW						0x100
#define POLYGON_FLAG_ZBIAS_MIDDLE					0x200
#define POLYGON_FLAG_ZBIAS_HIGH						0x300
#define POLYGON_FLAG_LIGHTNING_BOTHSIDES			0x20
#define POLYGON_FLAG_LIGHTNING_POSITION				0x80
#define POLYGON_FLAG_LIGHTNING_FLAT					0x200000
#define POLYGON_FLAG_LIGHTNING_REVERSED				0x100000

namespace libBISMod
{
	//Structures
	struct MLOD_LodHeader
	{
		uint8 signature[4];
		uint32 versionMajor;
		uint32 versionMinor;
	};

	struct MLOD_Polygon
	{
		uint32 type;
		struct
		{
			uint32 verticesIndex;
			uint32 normalsIndex;
			float32 u;
			float32 v;
		} vertexTables[4];
		uint32 flags;
		StdXX::String texturePath;
		StdXX::String materialName;
	};
}