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
#include <StdXXStreams.hpp>
//Local
#include <libBISMod/raP/raP.hpp>
#include "raPInternal.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;

//Local prototypes
static void ReadArrayValue(CRapArrayValue *pValue, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static uint32 ReadCompressedInteger(FileInputStream &file);
static uint32 ReadIndexedString(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static bool ReadNextPacket(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static void ReadVariable(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);

//Local functions
static void ReadArray(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	uint32 nElements, index;

	pNode->SetPacketType(RAP_PACKETTYPE_ARRAY);
	pNode->SetName(stringList[ReadIndexedString(stringList, file)]);
	nElements = ReadCompressedInteger(file);

	for(uint32 i = 0; i < nElements; i++)
	{
		index = pNode->AddArrayValue(CRapArrayValue());
		ReadArrayValue(&pNode->GetArrayValue(index), stringList, file);
	}
}

static void ReadArrayValue(CRapArrayValue *pValue, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte valueType;

	valueType = dataReader.ReadByte();

	switch(valueType)
	{
		case RAP_ARRAYTYPE_STRING:
			pValue->SetType(RAP_ARRAYTYPE_STRING);
			pValue->SetValue(stringList[ReadIndexedString(stringList, file)]);
			break;
		case RAP_ARRAYTYPE_FLOAT:
		{
			float f;

			pValue->SetType(RAP_ARRAYTYPE_FLOAT);
			f = dataReader.ReadFloat32();

			pValue->SetValue(f);
		}
			break;
		case RAP_ARRAYTYPE_INT:
		{
			int32 i;

			pValue->SetType(RAP_ARRAYTYPE_INT);
			i = dataReader.ReadInt32();

			pValue->SetValue(i);
		}
			break;
		case RAP_ARRAYTYPE_EMBEDDEDARRAY:
		{
			DynamicArray<CRapArrayValue> embeddedArray;
			uint32 nElements, index;

			pValue->SetType(RAP_ARRAYTYPE_EMBEDDEDARRAY);
			nElements = ReadCompressedInteger(file);

			for(uint32 i = 0; i < nElements; i++)
			{
				index = embeddedArray.Push(CRapArrayValue());
				ReadArrayValue(&embeddedArray[index],  stringList, file);
			}
			pValue->SetValue(embeddedArray);
		}
			break;
	}
}

static void ReadClass(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	TextReader textReader(file, TextCodecType::ASCII);

	uint32 nImbeddedPackets, index;

	pNode->SetPacketType(RAP_PACKETTYPE_CLASS);
	pNode->SetName(stringList[ReadIndexedString(stringList, file)]);
	pNode->SetInheritedClassName(textReader.ReadZeroTerminatedString());
	nImbeddedPackets = ReadCompressedInteger(file);

	for(uint32 i = 0; i < nImbeddedPackets; i++)
	{
		index = pNode->AddNode(CRapNode());
		ReadNextPacket(&pNode->GetNode(index), stringList, file);
	}
}

static uint32 ReadCompressedInteger(FileInputStream &file)
{
	DataReader dataReader(false, file);

	uint32 value;

	value = dataReader.ReadByte();
	if(value & 0x80)
	{
		value += (dataReader.ReadByte() - 1) * 0x80;
	}

	return value;
}

static uint32 ReadIndexedString(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	uint32 index;

	TextReader textReader(file, TextCodecType::ASCII);

	index = ReadCompressedInteger(file);
	if(!stringList.Contains(index))
	{
		stringList[index] = textReader.ReadZeroTerminatedString();
	}

	return index;
}

static bool ReadNextPacket(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte type = dataReader.ReadByte();

	switch(type)
	{
		case RAP_PACKETTYPE_CLASS:
			ReadClass(pNode, stringList, file);
			break;
		case RAP_PACKETTYPE_VARIABLE:
			ReadVariable(pNode, stringList, file);
			break;
		case RAP_PACKETTYPE_ARRAY:
			ReadArray(pNode, stringList, file);
			break;
		default:
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	return true;
}

static void ReadVariable(CRapNode *pNode, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte varType;

	varType = dataReader.ReadByte();
	pNode->SetPacketType(RAP_PACKETTYPE_VARIABLE);

	switch(varType)
	{
		case RAP_VARIABLETYPE_STRING:
			pNode->SetVariableType(RAP_VARIABLETYPE_STRING);
			pNode->SetName(stringList[ReadIndexedString(stringList, file)]);
			pNode->SetValue(stringList[ReadIndexedString(stringList, file)]);
			break;
		case RAP_VARIABLETYPE_FLOAT:
		{
			float f;

			pNode->SetVariableType(RAP_VARIABLETYPE_FLOAT);
			pNode->SetName(stringList[ReadIndexedString(stringList, file)]);
			f = dataReader.ReadFloat32();

			pNode->SetValue(f);
		}
			break;
		case RAP_VARIABLETYPE_INT:
		{
			int32 i;

			pNode->SetVariableType(RAP_VARIABLETYPE_INT);
			pNode->SetName(stringList[ReadIndexedString(stringList, file)]);
			i = dataReader.ReadInt32();

			pNode->SetValue(i);
		}
			break;
	}
}

//Namespace Functions
void libBISMod::ReadRapTreeFromFile(const Path& path, CRapTree *pRootNode)
{
	char signature[RAP_HEADER_SIGNATURELENGTH];
	char version[RAP_HEADER_VERSIONLENGTH];
	uint32 nDefs;

	FileInputStream file(path);
	DataReader dataReader(false, file);
	TextReader textReader(file, TextCodecType::ASCII);

	file.ReadBytes(&signature, RAP_HEADER_SIGNATURELENGTH);
	if(String::CopyUtf8Bytes(reinterpret_cast<const uint8 *>(signature), RAP_HEADER_SIGNATURELENGTH) != String::CopyUtf8Bytes(reinterpret_cast<const uint8 *>(RAP_HEADER_SIGNATURE), RAP_HEADER_SIGNATURELENGTH))
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
	file.ReadBytes(&version, RAP_HEADER_VERSIONLENGTH);
	if(String::CopyUtf8Bytes(reinterpret_cast<const uint8 *>(version), RAP_HEADER_VERSIONLENGTH) != String::CopyUtf8Bytes(reinterpret_cast<const uint8 *>(RAP_HEADER_VERSION), RAP_HEADER_VERSIONLENGTH))
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	ReadNextPacket(pRootNode, pRootNode->stringDefs, file);
	if(file.QueryRemainingBytes())
	{
		String buffer;
		uint32 value;

		nDefs = dataReader.ReadUInt32();
		for(uint32 i = 0; i < nDefs; i++)
		{
			buffer = textReader.ReadZeroTerminatedString();
			value = dataReader.ReadUInt32();
			pRootNode->intDefs[buffer] = value;
		}
	}

	if(file.QueryRemainingBytes())
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}
}