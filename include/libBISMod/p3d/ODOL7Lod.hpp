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
#include <StdXXCore.hpp>
#include <StdXXMath.hpp>
//Local
#include "P3DLod.hpp"

namespace libBISMod
{
	//Structures
	struct STransformMatrix
	{
		StdXX::Math::Vector3S rows[4];
	};

	struct SFrame
	{
		float frameTime;
		uint32 nBones;
		StdXX::Math::Vector3S *pBonePositions;
	};

	struct SLodEdges
	{
		uint32 nMLODEdges;
		uint16 *pMLODEdges; //Potentially compressed
		uint32 nODOLEdges;
		uint16 *pODOLEdges; //Potentially compressed
	};

	struct SLodProxy
	{
		char *pName;
		STransformMatrix matrix;
		uint32 proxySequenceId;
		uint32 namedSelectionIndex;
	};

	//Remove structure padding
#pragma pack(push)
#pragma pack(1)
	struct SLodSection
	{
		uint32 faceIndexOffsets[2];
		uint32 userValue;
		int16 textureIndex;
		uint32 polygonFlags;
	};
//Reset
#pragma pack(pop)

	struct SNamedProperty
	{
		char *pProperty;
		char *pValue;
	};

	struct SNamedSelection
	{
		char *name;
		uint32 nPolygons;
		uint16 *pPolygonIndices; //Potentially compressed
		uint32 nAlways0;
		uint32 *pAlways0Data;
		uint32 nUnknownData;
		uint32 *pUnknownData; //Potentially compressed
		uint8 isSectional;
		uint32 nSectionIndices;
		uint32 *pSectionIndices; //Potentially compressed
		uint32 nVertexTableIndices;
		uint16 *pVertexTableIndices; //Potentially compressed
		uint32 nVerticesWeights;
		byte *pVerticesWeights; //Potentially compressed
	};

	struct SPolygon
	{
		uint32 flags;
		uint16 textureIndex;
		byte type;
		uint16 *pVertexIndices;
	};

	struct SVertexTable
	{
		//nVerticesFlags == nUVPairs == nVertices == nNormals
		uint32 nVerticesFlags;
		uint32 *pVerticesFlags; //Potentially compressed
		uint32 nUVPairs;
		StdXX::Math::Vector2S *pUVPairs; //Potentially compressed
		uint32 nVertices;
		StdXX::Math::Vector3S *pVertices;
		uint32 nNormals;
		StdXX::Math::Vector3S *pNormals;
	};

	struct ODOL7LodData
	{
		SVertexTable vertexTable;
		float unknownFloat1;
		float unknownFloat2;
		StdXX::Math::Vector3S minPos;
		StdXX::Math::Vector3S maxPos;
		StdXX::Math::Vector3S autoCenterPos;
		float unknownFloat3;
		StdXX::DynamicArray<StdXX::String> textures;
		SLodEdges lodEdges;
		struct
		{
			uint32 nPolygons;
			uint32 offsetToLodSections;
			SPolygon *pPolygons;
		} polygons;
		struct
		{
			uint32 nSections;
			SLodSection *pSections;
		} lodSections;
		struct
		{
			uint32 nSelections;
			SNamedSelection *pSelections;
		} namedSelections;
		struct
		{
			uint32 nProperties;
			SNamedProperty *pProperties;
		} namedProperties;
		struct
		{
			uint32 nFrames;
			SFrame *pFrames;
		} frames;
		uint32 iconColor;
		uint32 selectedColor;
		uint32 unknown;
		struct
		{
			uint32 nProxies;
			SLodProxy *pProxies;
		} lodProxies;
	};

	class ODOL7Lod : public P3DLod
	{
	public:
		//State
		ODOL7LodData lodData;

		//Constructor
		inline ODOL7Lod(StdXX::InputStream& inputStream)
		{
			this->Read(inputStream);
		}

		//Destructor
		~ODOL7Lod();

		//Methods
		uint32 GetNumberOfPolygons() const override;
		void GetPolygon(uint32 index, P3DPolygon &polygon) const override;
		LodType GetType() const override;
		void Write(StdXX::OutputStream &outputStream) const override;

	private:
		//Methods
		void Read(StdXX::InputStream& inputStream);
		void ReadVertexTable(StdXX::InputStream& inputStream);
	};
}