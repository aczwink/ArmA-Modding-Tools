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
//Local
#include <libBISMod/raP/raP.hpp>
#include "raPInternal.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;

//Local prototypes
static void WriteRapString(const String& string, const RapTree& rootNode, BinaryTreeMap<String, uint32>& dictionary, OutputStream& outputStream);

//Local functions
static void WriteCompressedInteger(uint32 value, OutputStream& outputStream)
{
	DataWriter dataWriter(false, outputStream);

	ASSERT(value <= Signed<int16>::Max(), u8"TODO: can't write that high value. implement using loop?!")

	if(value < 0x80)
	{
		dataWriter.WriteByte(value);
	}
	else
	{
		dataWriter.WriteByte(value | 0x80);
		dataWriter.WriteByte(value >> 7);
	}
}

static void WriteIndexedString(const String& string, BinaryTreeMap<String, uint32>& dictionary, OutputStream& outputStream)
{
	auto it = dictionary.Find(string);
	if(it == dictionary.end())
	{
		uint32 nextStringId = dictionary.GetNumberOfElements();
		dictionary[string] = nextStringId;

		WriteCompressedInteger(nextStringId, outputStream);

		TextWriter textWriter(outputStream, TextCodecType::Latin1);
		textWriter.WriteStringZeroTerminated(string);
	}
	else
	{
		WriteCompressedInteger(it.operator*().value, outputStream);
	}
}

static void WriteRapArray(DynamicArray<RapArrayValue> &arrayValues, BinaryTreeMap<String, uint32>& dictionary, OutputStream& file)
{
	DataWriter dataWriter(false, file);

	for(uint32 i = 0; i < arrayValues.GetNumberOfElements(); i++)
	{
		dataWriter.WriteByte(arrayValues[i].Type());
		switch(arrayValues[i].Type())
		{
			case RAP_ARRAYTYPE_STRING:
				WriteIndexedString(arrayValues[i].ValueString(), dictionary, file);
				break;
			case RAP_ARRAYTYPE_FLOAT:
				dataWriter.WriteFloat32(arrayValues[i].GetValueFloat());
				break;
			case RAP_ARRAYTYPE_INT:
				dataWriter.WriteInt32(arrayValues[i].GetValueInt());
				break;
			case RAP_ARRAYTYPE_EMBEDDEDARRAY:
			{
				DynamicArray<RapArrayValue> subArray;

				arrayValues[i].GetValueArray(subArray);
				WriteCompressedInteger(subArray.GetNumberOfElements(), file);
				WriteRapArray(subArray, dictionary, file);
			}
				break;
		}
	}
}

static bool WriteRapPacket(const RapNode* pNode, const RapTree& rapTree, BinaryTreeMap<String, uint32>& dictionary, OutputStream& outputStream)
{
	DataWriter dataWriter(false, outputStream);

	switch(pNode->PacketType())
	{
		case RAP_PACKETTYPE_CLASS:
			dataWriter.WriteByte(RAP_PACKETTYPE_CLASS);
			WriteIndexedString(pNode->GetName(), dictionary, outputStream);
			if(pNode->InheritedClassName().IsEmpty())
			{
				dataWriter.WriteByte(0);
			}
			else
			{
				outputStream.WriteBytes(pNode->InheritedClassName().GetRawZeroTerminatedData(),
										pNode->InheritedClassName().GetLength()+1);
			}
			WriteCompressedInteger(pNode->GetNumberOfEmbeddedPackages(), outputStream);

			for(uint32 i = 0; i < pNode->GetNumberOfEmbeddedPackages(); i++)
			{
				WriteRapPacket(&pNode->GetChildNode(i), rapTree, dictionary, outputStream);
			}
			break;
		case RAP_PACKETTYPE_VARIABLE:
			dataWriter.WriteByte(RAP_PACKETTYPE_VARIABLE);
			dataWriter.WriteByte(pNode->VariableType());
			WriteIndexedString(pNode->GetName(), dictionary, outputStream);

			switch(pNode->VariableType())
			{
				case RAP_VARIABLETYPE_STRING:
					WriteRapString(pNode->VariableValueString(), rapTree, dictionary, outputStream);
					break;
				case RAP_VARIABLETYPE_FLOAT:
					dataWriter.WriteFloat32(pNode->GetVariableValueFloat());
					break;
				case RAP_VARIABLETYPE_INT:
					dataWriter.WriteInt32(pNode->GetVariableValueInt());
					break;
			}
			break;
		case RAP_PACKETTYPE_ARRAY:
		{
			DynamicArray<RapArrayValue> arrayValues;

			dataWriter.WriteByte(RAP_PACKETTYPE_ARRAY);
			WriteIndexedString(pNode->GetName(), dictionary, outputStream);

			arrayValues = pNode->ArrayValue();
			WriteCompressedInteger(arrayValues.GetNumberOfElements(), outputStream);
			WriteRapArray(arrayValues, dictionary, outputStream);
		}
			break;
	}

	return true;
}

static void WriteRapString(const String& string, const RapTree& rootNode, BinaryTreeMap<String, uint32>& dictionary, OutputStream& outputStream)
{
	WriteIndexedString(rootNode.IsEnumDefined(string) ? string.ToLowercase() : string, dictionary, outputStream);
}

//Namespace Functions
void libBISMod::SaveRapTreeToStream(OutputStream &outputStream, const RapTree& rapTree)
{
	BinaryTreeMap<String, uint32> dictionary;

	outputStream.WriteBytes(RAP_HEADER_SIGNATURE, RAP_HEADER_SIGNATURELENGTH);
	outputStream.WriteBytes(RAP_HEADER_VERSION, RAP_HEADER_VERSIONLENGTH);

	WriteRapPacket(rapTree.rootNode.operator->(), rapTree, dictionary, outputStream);

	DataWriter dataWriter(false, outputStream);
	TextWriter textWriter(outputStream, TextCodecType::ASCII);

	dataWriter.WriteUInt32(rapTree.EnumTable().GetNumberOfElements());
	for(const auto& entry : rapTree.EnumTable())
	{
		textWriter.WriteStringZeroTerminated(entry.key.ToLowercase());
		dataWriter.WriteUInt32(entry.value);
	}
}