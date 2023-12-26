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
static void ReadArrayValue(RapArrayValue *pValue, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static uint32 ReadCompressedInteger(FileInputStream &file);
static uint32 ReadIndexedString(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static UniquePointer<RapNode> ReadNextPacket(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);
static UniquePointer<RapNode> ReadVariable(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file);

//Local functions
static UniquePointer<RapNode> ReadArray(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	uint32 nElements, index;

	UniquePointer<RapNode> node = new RapNode;
	node->SetPacketType(RAP_PACKETTYPE_ARRAY);
	node->SetName(stringList[ReadIndexedString(stringList, file)]);
	nElements = ReadCompressedInteger(file);

	for(uint32 i = 0; i < nElements; i++)
	{
		index = node->AddArrayValue(RapArrayValue());
		ReadArrayValue(&node->GetArrayValue(index), stringList, file);
	}

	return node;
}

static void ReadArrayValue(RapArrayValue *pValue, BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte valueType;

	valueType = dataReader.ReadByte();

	switch(valueType)
	{
		case RAP_ARRAYTYPE_STRING:
			pValue->SetValue(stringList[ReadIndexedString(stringList, file)]);
			break;
		case RAP_ARRAYTYPE_FLOAT:
		{
			float32 f = dataReader.ReadFloat32();
			pValue->SetValue(f);
		}
		break;
		case RAP_ARRAYTYPE_INT:
		{
			int32 i = dataReader.ReadInt32();
			pValue->SetValue(i);
		}
		break;
		case RAP_ARRAYTYPE_EMBEDDEDARRAY:
		{
			DynamicArray<RapArrayValue> embeddedArray;
			uint32 nElements, index;

			nElements = ReadCompressedInteger(file);

			for(uint32 i = 0; i < nElements; i++)
			{
				index = embeddedArray.Push(RapArrayValue());
				ReadArrayValue(&embeddedArray[index], stringList, file);
			}
			pValue->SetValue(Move(embeddedArray));
		}
		break;
	}
}

static UniquePointer<RapNode> ReadClass(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	TextReader textReader(file, TextCodecType::ASCII);

	UniquePointer<RapNode> classNode = new RapNode;
	classNode->SetPacketType(RAP_PACKETTYPE_CLASS);
	classNode->SetName(stringList[ReadIndexedString(stringList, file)]);
	classNode->InheritedClassName(textReader.ReadZeroTerminatedString());
	uint32 nImbeddedPackets = ReadCompressedInteger(file);

	for(uint32 i = 0; i < nImbeddedPackets; i++)
	{
		auto child = ReadNextPacket(stringList, file);
		classNode->AddChild(Move(child));
	}

	return classNode;
}

static uint32 ReadCompressedInteger(FileInputStream &file)
{
	DataReader dataReader(false, file);

	uint32 value = dataReader.ReadByte();
	if(value & 0x80)
	{
		uint8 extra = dataReader.ReadByte();
		ASSERT((extra & 0x80) == 0, u8"TODO: implement long compressed integers")
		value += (extra - 1) * 0x80;
	}

	return value;
}

static uint32 ReadIndexedString(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	uint32 index;

	TextReader textReader(file, TextCodecType::Latin1);

	index = ReadCompressedInteger(file);
	if(!stringList.Contains(index))
	{
		stringList[index] = textReader.ReadZeroTerminatedString();
	}

	return index;
}

static UniquePointer<RapNode> ReadNextPacket(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte type = dataReader.ReadByte();

	switch(type)
	{
		case RAP_PACKETTYPE_CLASS:
			return ReadClass(stringList, file);
		case RAP_PACKETTYPE_VARIABLE:
			return ReadVariable(stringList, file);
		case RAP_PACKETTYPE_ARRAY:
			return ReadArray(stringList, file);
		default:
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	return nullptr;
}

static UniquePointer<RapNode> ReadVariable(BinaryTreeMap<uint32, String> &stringList, FileInputStream &file)
{
	DataReader dataReader(false, file);

	byte varType;

	varType = dataReader.ReadByte();
	UniquePointer<RapNode> node = new RapNode;
	node->SetPacketType(RAP_PACKETTYPE_VARIABLE);

	switch(varType)
	{
		case RAP_VARIABLETYPE_STRING:
			node->SetName(stringList[ReadIndexedString(stringList, file)]);
			node->SetValue(stringList[ReadIndexedString(stringList, file)]);
			break;
		case RAP_VARIABLETYPE_FLOAT:
		{
			float f;

			node->SetName(stringList[ReadIndexedString(stringList, file)]);
			f = dataReader.ReadFloat32();

			node->SetValue(f);
		}
			break;
		case RAP_VARIABLETYPE_INT:
		{
			int32 i;

			node->SetName(stringList[ReadIndexedString(stringList, file)]);
			i = dataReader.ReadInt32();

			node->SetValue(i);
		}
		break;
	}

	return node;
}

//Namespace Functions
UniquePointer<RapTree> libBISMod::ReadRapTreeFromFile(const Path& path)
{
	char signature[RAP_HEADER_SIGNATURELENGTH];
	char version[RAP_HEADER_VERSIONLENGTH];

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

	BinaryTreeMap<uint32, String> dictionary;
	auto rootNode = ReadNextPacket(dictionary, file);
	UniquePointer<RapTree> tree = new RapTree;
	tree->rootNode = Move(rootNode);

	//enum table
	uint32 nDefs = dataReader.ReadUInt32();
	for(uint32 i = 0; i < nDefs; i++)
	{
		String buffer = textReader.ReadZeroTerminatedString();
		uint32 value = dataReader.ReadUInt32();

		tree->DefineEnumValue(buffer, value);
	}

	if(file.QueryRemainingBytes())
	{
		NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	return tree;
}