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
#include <libBISMod/p3d/ODOL7Lod.hpp>
//Local
#include <libBISMod/p3d/MLOD_Lod.hpp>
#include <libBISMod/p3d/MLOD.hpp>
#include <libBISMod/p3d/ODOL7ModelInfo.hpp>
#include "LZSS.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::Math;
//Definitions
#define repeat(count, iteratorVarName) for(uint32 iteratorVarName = 0; iteratorVarName < count; iteratorVarName++)
#define WRITECOMPRESSED(pData) \
	if(uncompressedSize > P3D_GAPSIZE_COMPRESSION) \
	{ \
		pCompressedData = (byte *)malloc(uncompressedSize * 2); /*lame because compression does not work correctly*/ \
		LZSSCompressData((byte *)pData, uncompressedSize, pCompressedData, compressedSize); \
		dataWriter.WriteBytes(pCompressedData, compressedSize); \
		free(pCompressedData); \
	} \
	else \
	{ \
		dataWriter.WriteBytes(pData, uncompressedSize); \
	}

//Destructor
ODOL7Lod::~ODOL7Lod()
{
	//Vertex Table
	free(this->lodData.vertexTable.pVerticesFlags);
	free(this->lodData.vertexTable.pUVPairs);
	free(this->lodData.vertexTable.pVertices);
	free(this->lodData.vertexTable.pNormals);

	//Lod Edges
	free(this->lodData.lodEdges.pMLODEdges);
	free(this->lodData.lodEdges.pODOLEdges);

	//Polygons
	repeat(this->lodData.faces.nFaces, i)
	{
		free(this->lodData.faces.pPolygons[i].pVertexIndices);
	}
	free(this->lodData.faces.pPolygons);

	//Lod Sections
	free(this->lodData.lodSections.pSections);

	//Named Selections
	repeat(this->lodData.namedSelections.nSelections, i)
	{
		free(this->lodData.namedSelections.pSelections[i].name);
		free(this->lodData.namedSelections.pSelections[i].pPolygonIndices);
		free(this->lodData.namedSelections.pSelections[i].pUnknownData);
		free(this->lodData.namedSelections.pSelections[i].pSectionIndices);
		free(this->lodData.namedSelections.pSelections[i].pVertexTableIndices);
		free(this->lodData.namedSelections.pSelections[i].pVerticesWeights);
	}
	free(this->lodData.namedSelections.pSelections);

	//Named Properties
	repeat(this->lodData.namedProperties.nProperties, i)
	{
		free(this->lodData.namedProperties.pProperties[i].pProperty);
		free(this->lodData.namedProperties.pProperties[i].pValue);
	}
	free(this->lodData.namedProperties.pProperties);

	//Frames
	repeat(this->lodData.frames.nFrames, i)
	{
		free(this->lodData.frames.pFrames[i].pBonePositions);
	}
	free(this->lodData.frames.pFrames);

	//Lod Proxies
	repeat(this->lodData.lodProxies.nProxies, i)
	{
		free(this->lodData.lodProxies.pProxies[i].pName);
	}
	free(this->lodData.lodProxies.pProxies);
}

//Public methods
uint32 ODOL7Lod::GetNumberOfPolygons() const
{
	return this->lodData.faces.nFaces;
}

void ODOL7Lod::GetPolygon(uint32 index, P3DPolygon &polygon) const
{
	uint16 textureIndex = this->lodData.faces.pPolygons[index].textureIndex;
	polygon.texturePath = this->lodData.textures[textureIndex];
}

LodType ODOL7Lod::GetType() const
{
	return LodType::ODOL7;
}

void ODOL7Lod::Write(OutputStream &outputStream) const
{
	byte *pCompressedData;
	uint32 uncompressedSize, compressedSize;

	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	//VertexTable
	dataWriter.WriteUInt32(this->lodData.vertexTable.nVerticesFlags);
	uncompressedSize = this->lodData.vertexTable.nVerticesFlags * sizeof(*this->lodData.vertexTable.pVerticesFlags);
	WRITECOMPRESSED(this->lodData.vertexTable.pVerticesFlags);

	dataWriter.WriteUInt32(this->lodData.vertexTable.nUVPairs);
	uncompressedSize = this->lodData.vertexTable.nUVPairs * sizeof(*this->lodData.vertexTable.pUVPairs);
	WRITECOMPRESSED(this->lodData.vertexTable.pUVPairs);

	dataWriter.WriteUInt32(this->lodData.vertexTable.nVertices);
	dataWriter.WriteBytes(this->lodData.vertexTable.pVertices, this->lodData.vertexTable.nVertices * sizeof(*this->lodData.vertexTable.pVertices));

	dataWriter.WriteUInt32(this->lodData.vertexTable.nNormals);
	dataWriter.WriteBytes(this->lodData.vertexTable.pNormals, this->lodData.vertexTable.nNormals * sizeof(*this->lodData.vertexTable.pNormals));

	//Values
	dataWriter.WriteFloat32(this->lodData.unknownFloat1);
	dataWriter.WriteFloat32(this->lodData.unknownFloat2);
	dataWriter.WriteBytes(&this->lodData.minPos, sizeof(this->lodData.minPos));
	dataWriter.WriteBytes(&this->lodData.maxPos, sizeof(this->lodData.maxPos));
	dataWriter.WriteBytes(&this->lodData.autoCenterPos, sizeof(this->lodData.autoCenterPos));
	dataWriter.WriteFloat32(this->lodData.unknownFloat3);

	//Textures
	dataWriter.WriteUInt32(this->lodData.textures.GetNumberOfElements());
	for(const auto& texture : this->lodData.textures)
		textWriter.WriteStringZeroTerminated(texture);

	//Lod Edges
	dataWriter.WriteUInt32(this->lodData.lodEdges.nMLODEdges);
	uncompressedSize = this->lodData.lodEdges.nMLODEdges * sizeof(*this->lodData.lodEdges.pMLODEdges);
	WRITECOMPRESSED(this->lodData.lodEdges.pMLODEdges);
	dataWriter.WriteUInt32(this->lodData.lodEdges.nODOLEdges);
	uncompressedSize = this->lodData.lodEdges.nODOLEdges * sizeof(*this->lodData.lodEdges.pODOLEdges);
	WRITECOMPRESSED(this->lodData.lodEdges.pODOLEdges);

	//Faces
	dataWriter.WriteUInt32(this->lodData.faces.nFaces);
	dataWriter.WriteUInt32(this->lodData.faces.offsetToLodSections);

	repeat(this->lodData.faces.nFaces, i)
	{
		dataWriter.WriteUInt32(this->lodData.faces.pPolygons[i].flags);
		dataWriter.WriteUInt16(this->lodData.faces.pPolygons[i].textureIndex);
		dataWriter.WriteByte(this->lodData.faces.pPolygons[i].type);
		dataWriter.WriteBytes(this->lodData.faces.pPolygons[i].pVertexIndices, this->lodData.faces.pPolygons[i].type * sizeof(*this->lodData.faces.pPolygons[i].pVertexIndices));
	}

	//Lod Sections
	dataWriter.WriteUInt32(this->lodData.lodSections.nSections);
	dataWriter.WriteBytes(this->lodData.lodSections.pSections, this->lodData.lodSections.nSections * sizeof(*this->lodData.lodSections.pSections));

	//Named Selections
	dataWriter.WriteUInt32(this->lodData.namedSelections.nSelections);
	repeat(this->lodData.namedSelections.nSelections, i)
	{
		dataWriter.WriteBytes(this->lodData.namedSelections.pSelections[i].name, GetStringLength(this->lodData.namedSelections.pSelections[i].name)+1);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nPolygons);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nPolygons * sizeof(*this->lodData.namedSelections.pSelections[i].pPolygonIndices);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pPolygonIndices);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nAlways0);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nAlways0 * sizeof(*this->lodData.namedSelections.pSelections[i].pAlways0Data);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pAlways0Data);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nUnknownData);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nUnknownData * sizeof(*this->lodData.namedSelections.pSelections[i].pUnknownData);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pUnknownData);

		dataWriter.WriteByte(this->lodData.namedSelections.pSelections[i].isSectional);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nSectionIndices);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nSectionIndices * sizeof(*this->lodData.namedSelections.pSelections[i].pSectionIndices);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pSectionIndices);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nVertexTableIndices);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nVertexTableIndices * sizeof(*this->lodData.namedSelections.pSelections[i].pVertexTableIndices);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pVertexTableIndices);

		dataWriter.WriteUInt32(this->lodData.namedSelections.pSelections[i].nVerticesWeights);
		uncompressedSize = this->lodData.namedSelections.pSelections[i].nVerticesWeights * sizeof(*this->lodData.namedSelections.pSelections[i].pVerticesWeights);
		WRITECOMPRESSED(this->lodData.namedSelections.pSelections[i].pVerticesWeights);
	}

	//Named Properties
	dataWriter.WriteUInt32(this->lodData.namedProperties.nProperties);
	repeat(this->lodData.namedProperties.nProperties, i)
	{
		dataWriter.WriteBytes(this->lodData.namedProperties.pProperties[i].pProperty, GetStringLength(this->lodData.namedProperties.pProperties[i].pProperty)+1);
		dataWriter.WriteBytes(this->lodData.namedProperties.pProperties[i].pValue, GetStringLength(this->lodData.namedProperties.pProperties[i].pValue)+1);
	}

	//Frames
	dataWriter.WriteUInt32(this->lodData.frames.nFrames);
	repeat(this->lodData.frames.nFrames, i)
	{
		dataWriter.WriteFloat32(this->lodData.frames.pFrames[i].frameTime);
		dataWriter.WriteUInt32(this->lodData.frames.pFrames[i].nBones);
		dataWriter.WriteBytes(this->lodData.frames.pFrames[i].pBonePositions, this->lodData.frames.pFrames[i].nBones * sizeof(*this->lodData.frames.pFrames[i].pBonePositions));
	}

	//Second Values
	dataWriter.WriteUInt32(this->lodData.iconColor);
	dataWriter.WriteUInt32(this->lodData.selectedColor);
	dataWriter.WriteUInt32(this->lodData.unknown);

	//Proxies
	dataWriter.WriteUInt32(this->lodData.lodProxies.nProxies);
	repeat(this->lodData.lodProxies.nProxies, i)
	{
		dataWriter.WriteBytes(this->lodData.lodProxies.pProxies[i].pName, GetStringLength(this->lodData.lodProxies.pProxies[i].pName)+1);
		dataWriter.WriteBytes(&this->lodData.lodProxies.pProxies[i].matrix, sizeof(this->lodData.lodProxies.pProxies[i].matrix));
		dataWriter.WriteUInt32(this->lodData.lodProxies.pProxies[i].proxySequenceId);
		dataWriter.WriteUInt32(this->lodData.lodProxies.pProxies[i].namedSelectionIndex);
	}
}

//Private methods
void ODOL7Lod::Read(InputStream& inputStream)
{
	DataReader dataReader(false, inputStream);
	TextReader textReader(inputStream, TextCodecType::ASCII);

	this->ReadVertexTable(inputStream);

	this->lodData.unknownFloat1 = dataReader.ReadFloat32();
	this->lodData.unknownFloat2 = dataReader.ReadFloat32();
	dataReader.ReadBytes(&this->lodData.minPos, sizeof(this->lodData.minPos));
	dataReader.ReadBytes(&this->lodData.maxPos, sizeof(this->lodData.maxPos));
	dataReader.ReadBytes(&this->lodData.autoCenterPos, sizeof(this->lodData.autoCenterPos));
	this->lodData.unknownFloat3 = dataReader.ReadFloat32();

	//Textures
	this->lodData.textures.Resize(dataReader.ReadUInt32());
	for(auto& texture : this->lodData.textures)
		texture = textReader.ReadZeroTerminatedString();

	//Lod Edges
	this->lodData.lodEdges.nMLODEdges = dataReader.ReadUInt32();
	uint32 size = this->lodData.lodEdges.nMLODEdges * sizeof(*this->lodData.lodEdges.pMLODEdges);
	this->lodData.lodEdges.pMLODEdges = (uint16 *)malloc(size);
	if(size > P3D_GAPSIZE_COMPRESSION)
	{
		LZSSReadCompressed((byte *)this->lodData.lodEdges.pMLODEdges, size, inputStream);
	}
	else
	{
		dataReader.ReadBytes(this->lodData.lodEdges.pMLODEdges, size);
	}

	this->lodData.lodEdges.nODOLEdges = dataReader.ReadUInt32();
	size = this->lodData.lodEdges.nODOLEdges * sizeof(*this->lodData.lodEdges.pODOLEdges);
	this->lodData.lodEdges.pODOLEdges = (uint16 *)malloc(size);
	if(size > P3D_GAPSIZE_COMPRESSION)
	{
		LZSSReadCompressed((byte *)this->lodData.lodEdges.pODOLEdges, size, inputStream);
	}
	else
	{
		dataReader.ReadBytes(this->lodData.lodEdges.pODOLEdges, size);
	}

	//Polygons
	this->lodData.faces.nFaces = dataReader.ReadUInt32();
	this->lodData.faces.offsetToLodSections = dataReader.ReadUInt32();
	this->lodData.faces.pPolygons = (SPolygon *)malloc(this->lodData.faces.nFaces * sizeof(*this->lodData.faces.pPolygons));
	repeat(this->lodData.faces.nFaces, i)
	{
		this->lodData.faces.pPolygons[i].flags = dataReader.ReadUInt32();
		this->lodData.faces.pPolygons[i].textureIndex = dataReader.ReadUInt16();
		this->lodData.faces.pPolygons[i].type = dataReader.ReadByte();
		size = this->lodData.faces.pPolygons[i].type * sizeof(*this->lodData.faces.pPolygons[i].pVertexIndices);
		this->lodData.faces.pPolygons[i].pVertexIndices = (uint16 *)malloc(size);
		dataReader.ReadBytes(this->lodData.faces.pPolygons[i].pVertexIndices, size);
	}

	//Lod Sections
	this->lodData.lodSections.nSections = dataReader.ReadUInt32();
	this->lodData.lodSections.pSections = (SLodSection *)malloc(this->lodData.lodSections.nSections * sizeof(*this->lodData.lodSections.pSections));
	repeat(this->lodData.lodSections.nSections, i)
	{
		this->lodData.lodSections.pSections[i].faceIndexOffsets[0] = dataReader.ReadUInt32();
		this->lodData.lodSections.pSections[i].faceIndexOffsets[1] = dataReader.ReadUInt32();
		this->lodData.lodSections.pSections[i].userValue = dataReader.ReadUInt32();
		this->lodData.lodSections.pSections[i].textureIndex = dataReader.ReadInt16();
		this->lodData.lodSections.pSections[i].polygonFlags = dataReader.ReadUInt32();
	}

	//Named Selections
	this->lodData.namedSelections.nSelections = dataReader.ReadUInt32();
	this->lodData.namedSelections.pSelections = (SNamedSelection *)malloc(this->lodData.namedSelections.nSelections * sizeof(*this->lodData.namedSelections.pSelections));
	repeat(this->lodData.namedSelections.nSelections, i)
	{
		String buffer;

		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.namedSelections.pSelections[i].name = (char *)malloc(buffer.GetLength()+1);
		this->lodData.namedSelections.pSelections[i].name[buffer.GetLength()] = '\0';
		MemCopy(this->lodData.namedSelections.pSelections[i].name, buffer.GetRawZeroTerminatedData(), buffer.GetLength());

		this->lodData.namedSelections.pSelections[i].nPolygons = dataReader.ReadUInt32();
		size = this->lodData.namedSelections.pSelections[i].nPolygons * sizeof(*this->lodData.namedSelections.pSelections[i].pPolygonIndices);
		this->lodData.namedSelections.pSelections[i].pPolygonIndices = (uint16 *)malloc(size);
		if(size > P3D_GAPSIZE_COMPRESSION)
		{
			LZSSReadCompressed((byte *)this->lodData.namedSelections.pSelections[i].pPolygonIndices, size, inputStream);
		}
		else
		{
			dataReader.ReadBytes(this->lodData.namedSelections.pSelections[i].pPolygonIndices, size);
		}

		this->lodData.namedSelections.pSelections[i].nAlways0 = dataReader.ReadUInt32();
		this->lodData.namedSelections.pSelections[i].pAlways0Data = NULL;
		if(this->lodData.namedSelections.pSelections[i].nAlways0 != 0)
		{
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
			return;
		}

		this->lodData.namedSelections.pSelections[i].nUnknownData = dataReader.ReadUInt32();
		size = this->lodData.namedSelections.pSelections[i].nUnknownData * sizeof(*this->lodData.namedSelections.pSelections[i].pUnknownData);
		this->lodData.namedSelections.pSelections[i].pUnknownData = (uint32 *)malloc(size);
		if(size > P3D_GAPSIZE_COMPRESSION)
		{
			LZSSReadCompressed((byte *)this->lodData.namedSelections.pSelections[i].pUnknownData, size, inputStream);
		}
		else
		{
			dataReader.ReadBytes(this->lodData.namedSelections.pSelections[i].pUnknownData, size);
		}

		this->lodData.namedSelections.pSelections[i].isSectional = dataReader.ReadByte();

		this->lodData.namedSelections.pSelections[i].nSectionIndices = dataReader.ReadUInt32();
		size = this->lodData.namedSelections.pSelections[i].nSectionIndices * sizeof(*this->lodData.namedSelections.pSelections[i].pSectionIndices);
		this->lodData.namedSelections.pSelections[i].pSectionIndices = (uint32 *)malloc(size);
		if(size > P3D_GAPSIZE_COMPRESSION)
		{
			LZSSReadCompressed((byte *)this->lodData.namedSelections.pSelections[i].pSectionIndices, size, inputStream);
		}
		else
		{
			dataReader.ReadBytes(this->lodData.namedSelections.pSelections[i].pSectionIndices, size);
		}

		this->lodData.namedSelections.pSelections[i].nVertexTableIndices = dataReader.ReadUInt32();
		size = this->lodData.namedSelections.pSelections[i].nVertexTableIndices * sizeof(*this->lodData.namedSelections.pSelections[i].pVertexTableIndices);
		this->lodData.namedSelections.pSelections[i].pVertexTableIndices = (uint16 *)malloc(size);
		if(size > P3D_GAPSIZE_COMPRESSION)
		{
			LZSSReadCompressed((byte *)this->lodData.namedSelections.pSelections[i].pVertexTableIndices, size, inputStream);
		}
		else
		{
			dataReader.ReadBytes(this->lodData.namedSelections.pSelections[i].pVertexTableIndices, size);
		}

		this->lodData.namedSelections.pSelections[i].nVerticesWeights = dataReader.ReadUInt32();
		size = this->lodData.namedSelections.pSelections[i].nVerticesWeights * sizeof(*this->lodData.namedSelections.pSelections[i].pVerticesWeights);
		this->lodData.namedSelections.pSelections[i].pVerticesWeights = (byte *)malloc(size);
		if(size > P3D_GAPSIZE_COMPRESSION)
		{
			LZSSReadCompressed((byte *)this->lodData.namedSelections.pSelections[i].pVerticesWeights, size, inputStream);
		}
		else
		{
			dataReader.ReadBytes(this->lodData.namedSelections.pSelections[i].pVerticesWeights, size);
		}
	}

	//Named Properties
	this->lodData.namedProperties.nProperties = dataReader.ReadUInt32();
	this->lodData.namedProperties.pProperties = (SNamedProperty *)malloc(this->lodData.namedProperties.nProperties * sizeof(*this->lodData.namedProperties.pProperties));
	repeat(this->lodData.namedProperties.nProperties, i)
	{
		String buffer;

		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.namedProperties.pProperties[i].pProperty = (char *)malloc(buffer.GetLength()+1);
		this->lodData.namedProperties.pProperties[i].pProperty[buffer.GetLength()] = '\0';
		MemCopy(this->lodData.namedProperties.pProperties[i].pProperty, buffer.GetRawZeroTerminatedData(), buffer.GetLength());
		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.namedProperties.pProperties[i].pValue = (char *)malloc(buffer.GetLength()+1);
		this->lodData.namedProperties.pProperties[i].pValue[buffer.GetLength()] = '\0';
		MemCopy(this->lodData.namedProperties.pProperties[i].pValue, buffer.GetRawZeroTerminatedData(), buffer.GetLength());
	}

	//Frames
	this->lodData.frames.nFrames = dataReader.ReadUInt32();
	this->lodData.frames.pFrames = (SFrame *)malloc(this->lodData.frames.nFrames * sizeof(*this->lodData.frames.pFrames));
	repeat(this->lodData.frames.nFrames, i)
	{
		this->lodData.frames.pFrames[i].frameTime = dataReader.ReadFloat32();
		this->lodData.frames.pFrames[i].nBones = dataReader.ReadUInt32();
		size = this->lodData.frames.pFrames[i].nBones * sizeof(*this->lodData.frames.pFrames[i].pBonePositions);
		this->lodData.frames.pFrames[i].pBonePositions = (Vector3S *)malloc(size);
		dataReader.ReadBytes(this->lodData.frames.pFrames[i].pBonePositions, size);
	}

	this->lodData.iconColor = dataReader.ReadUInt32();
	this->lodData.selectedColor = dataReader.ReadUInt32();
	this->lodData.unknown = dataReader.ReadUInt32();

	//Proxies
	this->lodData.lodProxies.nProxies = dataReader.ReadUInt32();
	this->lodData.lodProxies.pProxies = (SLodProxy *)malloc(this->lodData.lodProxies.nProxies * sizeof(*this->lodData.lodProxies.pProxies));
	repeat(this->lodData.lodProxies.nProxies, i)
	{
		String buffer;

		buffer = textReader.ReadZeroTerminatedString();
		this->lodData.lodProxies.pProxies[i].pName = (char *)malloc(buffer.GetLength()+1);
		this->lodData.lodProxies.pProxies[i].pName[buffer.GetLength()] = '\0';
		MemCopy(this->lodData.lodProxies.pProxies[i].pName, buffer.GetRawZeroTerminatedData(), buffer.GetLength());

		dataReader.ReadBytes(&this->lodData.lodProxies.pProxies[i].matrix, sizeof(this->lodData.lodProxies.pProxies[i].matrix));
		this->lodData.lodProxies.pProxies[i].proxySequenceId = dataReader.ReadUInt32();
		this->lodData.lodProxies.pProxies[i].namedSelectionIndex = dataReader.ReadUInt32();
	}
}

void ODOL7Lod::ReadVertexTable(InputStream& inputStream)
{
	DataReader dataReader(false, inputStream);

	this->lodData.vertexTable.nVerticesFlags = dataReader.ReadUInt32();
	uint32 size = this->lodData.vertexTable.nVerticesFlags * sizeof(*this->lodData.vertexTable.pVerticesFlags);
	this->lodData.vertexTable.pVerticesFlags = (uint32 *)malloc(size);
	if(size > P3D_GAPSIZE_COMPRESSION)
	{
		LZSSReadCompressed((byte *)this->lodData.vertexTable.pVerticesFlags, size, inputStream);
	}
	else
	{
		dataReader.ReadBytes(this->lodData.vertexTable.pVerticesFlags, size);
	}

	this->lodData.vertexTable.nUVPairs = dataReader.ReadUInt32();
	size = this->lodData.vertexTable.nUVPairs * sizeof(*this->lodData.vertexTable.pUVPairs);
	this->lodData.vertexTable.pUVPairs = (Vector2S *)malloc(size);
	if(size > P3D_GAPSIZE_COMPRESSION)
	{
		LZSSReadCompressed((byte *)this->lodData.vertexTable.pUVPairs, size, inputStream);
	}
	else
	{
		dataReader.ReadBytes(this->lodData.vertexTable.pUVPairs, size);
	}

	this->lodData.vertexTable.nVertices = dataReader.ReadUInt32();
	size = this->lodData.vertexTable.nVertices * sizeof(*this->lodData.vertexTable.pVertices);
	this->lodData.vertexTable.pVertices = (Vector3S *)malloc(size);
	dataReader.ReadBytes(this->lodData.vertexTable.pVertices, size);

	this->lodData.vertexTable.nNormals = dataReader.ReadUInt32();
	size = this->lodData.vertexTable.nNormals * sizeof(*this->lodData.vertexTable.pNormals);
	this->lodData.vertexTable.pNormals = (Vector3S *)malloc(size);
	dataReader.ReadBytes(this->lodData.vertexTable.pNormals, size);
}
