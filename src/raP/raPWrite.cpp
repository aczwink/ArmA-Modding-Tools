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
static void WriteCompressedInteger(int32 value, FileOutputStream &file);
static void WriteIndexedString(const String &str, uint32 &nextStringId, BinaryTreeMap<uint32, String> &strings, FileOutputStream &file);

//Local functions
static void WriteRapArray(DynamicArray<CRapArrayValue> &arrayValues, BinaryTreeMap<uint32, String> &strings, uint32 &nextStringId, FileOutputStream &file)
{
	DataWriter dataWriter(false, file);

	for(uint32 i = 0; i < arrayValues.GetNumberOfElements(); i++)
	{
		dataWriter.WriteByte(arrayValues[i].GetType());
		switch(arrayValues[i].GetType())
		{
			case RAP_ARRAYTYPE_STRING:
				WriteIndexedString(arrayValues[i].GetValueString(), nextStringId, strings, file);
				break;
			case RAP_ARRAYTYPE_FLOAT:
				dataWriter.WriteFloat32(arrayValues[i].GetValueFloat());
				break;
			case RAP_ARRAYTYPE_INT:
				dataWriter.WriteInt32(arrayValues[i].GetValueInt());
				break;
			case RAP_ARRAYTYPE_EMBEDDEDARRAY:
			{
				DynamicArray<CRapArrayValue> subArray;

				arrayValues[i].GetValueArray(subArray);
				WriteCompressedInteger(subArray.GetNumberOfElements(), file);
				WriteRapArray(subArray, strings, nextStringId, file);
			}
				break;
		}
	}
}

static void WriteCompressedInteger(int32 value, FileOutputStream &file)
{
	DataWriter dataWriter(false, file);

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

static void WriteIndexedString(const String &str, uint32 &nextStringId, BinaryTreeMap<uint32, String> &strings, FileOutputStream &file)
{
	Optional<uint32> key;
	for(const auto& entry : strings)
	{
		if(str == entry.value)
		{
			key = entry.key;
			break;
		}
	}

	if(key.HasValue())
	{
		WriteCompressedInteger(*key, file);
	}
	else
	{
		strings[nextStringId] = str;
		auto it = strings.Find(nextStringId++);
		WriteCompressedInteger((*it).key, file);
		file.WriteBytes((*it).value.GetRawZeroTerminatedData(), (*it).value.GetLength()+1);
	}
}

static bool WriteRapPacket(CRapNode *pNode, BinaryTreeMap<uint32, String> &strings, uint32 &nextStringId, FileOutputStream &file)
{
	DataWriter dataWriter(false, file);

	switch(pNode->GetPacketType())
	{
		case RAP_PACKETTYPE_CLASS:
			dataWriter.WriteByte(RAP_PACKETTYPE_CLASS);
			WriteIndexedString(pNode->GetName(), nextStringId, strings, file);
			if(pNode->GetInheritedClassName().IsEmpty())
			{
				dataWriter.WriteByte(0);
			}
			else
			{
				file.WriteBytes(pNode->GetInheritedClassName().GetRawZeroTerminatedData(), pNode->GetInheritedClassName().GetLength()+1);
			}
			WriteCompressedInteger(pNode->GetNumberOfEmbeddedPackages(), file);

			for(uint32 i = 0; i < pNode->GetNumberOfEmbeddedPackages(); i++)
			{
				WriteRapPacket(&pNode->GetNode(i), strings, nextStringId, file);
			}
			break;
		case RAP_PACKETTYPE_VARIABLE:
			dataWriter.WriteByte(RAP_PACKETTYPE_VARIABLE);
			dataWriter.WriteByte(pNode->GetVariableType());
			WriteIndexedString(pNode->GetName(), nextStringId, strings, file);

			switch(pNode->GetVariableType())
			{
				case RAP_VARIABLETYPE_STRING:
					WriteIndexedString(pNode->GetVariableValueString(), nextStringId, strings, file);
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
			DynamicArray<CRapArrayValue> arrayValues;

			dataWriter.WriteByte(RAP_PACKETTYPE_ARRAY);
			WriteIndexedString(pNode->GetName(), nextStringId, strings, file);

			pNode->GetVariableValueArray(arrayValues);
			WriteCompressedInteger(arrayValues.GetNumberOfElements(), file);
			WriteRapArray(arrayValues, strings, nextStringId, file);
		}
			break;
	}

	return true;
}

//Namespace Functions
void libBISMod::SaveRapTreeToFile(const Path& path, CRapTree *pRootNode)
{
	BinaryTreeMap<uint32, String> strings;
	uint32 nextStringId = 0;

	FileOutputStream file(path);

	file.WriteBytes(RAP_HEADER_SIGNATURE, RAP_HEADER_SIGNATURELENGTH);
	file.WriteBytes(RAP_HEADER_VERSION, RAP_HEADER_VERSIONLENGTH);

	WriteRapPacket(pRootNode, strings, nextStringId, file);
	pRootNode->stringDefs = strings;
}