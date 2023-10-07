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
#include <libBISMod/raP/RapNode.hpp>

//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;
//Definitions
#define PUTTABS for(uint32 i = 0; i < nTabs; i++){ textWriter << u8'\t'; }

//Local functions
static uint32 CalcCommonPrefixLength(const String& a, const String& b)
{
	auto it = a.begin();
	auto it2 = b.begin();
	uint32 pos = 0;

	while((it != a.end()) and (it2 != b.end()))
	{
		if(*it != *it2)
			break;
		++it;
		++it2;
		pos++;
	}

	return pos;
}

static String CalcCommonPrefix(const DynamicArray<String>& strings)
{
	String prefix = strings[0];
	for(uint32 i = 1; i < strings.GetNumberOfElements(); i++)
	{
		uint32 length = CalcCommonPrefixLength(prefix, strings[i]);
		prefix = prefix.SubString(0, length);
	}
	return prefix;
}

static void AddEnumValues(DynamicArray<DynamicArray<String>>& enums, BinaryTreeMap<String, uint32>& remaining, uint32 enumValue)
{
	DynamicArray<String> toRemove;
	for(const auto& kv : remaining)
	{
		if(kv.key.IsEmpty())
		{
			//happens in default CONFIG.BIN of ArmA CWA 1.99 with ManActN. Probably it is empty string, because it is not referenced anywhere?!
			toRemove.Push(kv.key);
			continue;
		}

		if(kv.value == enumValue)
		{
			if(enumValue == 0)
			{
				DynamicArray<String> newEnum;
				newEnum.Push(kv.key);

				enums.Push(Move(newEnum));
			}
			else
			{
				uint32 bestEnumIndex = 0;
				int32 bestLength = -1;
				for(uint32 i = 0; i < enums.GetNumberOfElements(); i++)
				{
					if(enums[i].GetNumberOfElements() != enumValue)
						continue;

					uint32 prefixLength = CalcCommonPrefixLength(enums[i][0], kv.key);
					if(int32(prefixLength) > bestLength)
					{
						bestEnumIndex = i;
						bestLength = prefixLength;
					}
				}

				enums[bestEnumIndex].Push(kv.key);
			}

			toRemove.Push(kv.key);
		}
	}

	for(const auto& s : toRemove)
		remaining.Remove(s);
}

static void WriteEnumTable(const BinaryTreeMap<String, uint32>& enumTable, TextWriter& textWriter)
{
	DynamicArray<DynamicArray<String>> enums;
	BinaryTreeMap<String, uint32> remaining = enumTable;

	for(uint32 enumValue = 0; !remaining.IsEmpty(); enumValue++)
	{
		AddEnumValues(enums, remaining, enumValue);
	}

	for(const auto& enumDef : enums)
	{
		textWriter << u8"enum " << CalcCommonPrefix(enumDef) << endl << u8"{" << endl;
		for(const auto& entry : enumDef)
		{
			textWriter << u8"\t" << entry << u8"," << endl;
		}
		textWriter << u8"};" << endl;
	}
}

static void WriteRawRapArrayValue(RapArrayValue *pValue, TextWriter& textWriter)
{
	switch(pValue->GetType())
	{
		case RAP_ARRAYTYPE_STRING:
			textWriter << u8'"' << pValue->GetValueString() << u8'"';
			break;
		case RAP_ARRAYTYPE_FLOAT:
			textWriter << pValue->GetValueFloat();
			break;
		case RAP_ARRAYTYPE_INT:
			textWriter << pValue->GetValueInt();
			break;
		case RAP_ARRAYTYPE_EMBEDDEDARRAY:
		{
			DynamicArray<RapArrayValue> array;

			pValue->GetValueArray(array);
			textWriter << u8'{';
			for(uint32 i = 0; i < array.GetNumberOfElements(); i++)
			{
				WriteRawRapArrayValue(&array[i], textWriter);
				if(i+1 != array.GetNumberOfElements())
				{
					textWriter << u8", ";
				}
			}
			textWriter << u8'}';
		}
			break;
	}
}

static void WriteRawRapPacket(const RapNode& node, uint16 nTabs, TextWriter& textWriter)
{
	PUTTABS;

	switch(node.GetPacketType())
	{
		case RAP_PACKETTYPE_CLASS:
			textWriter << "class " << node.GetName();
			if(!node.GetInheritedClassName().IsEmpty())
			{
				textWriter << " : " << node.GetInheritedClassName();
			}
			textWriter << endl;
			PUTTABS;
			textWriter << u8'{' << endl;
			for(uint32 i = 0; i < node.GetNumberOfEmbeddedPackages(); i++)
			{
				WriteRawRapPacket(node.GetChildNode(i), nTabs+1, textWriter);
				if(i+1 != node.GetNumberOfEmbeddedPackages())
				{
					if(node.GetChildNode(i+1).GetPacketType() == RAP_PACKETTYPE_CLASS)
					{
						textWriter << endl;
					}
				}
			}
			PUTTABS;
			textWriter << "};" << endl;
			break;
		case RAP_PACKETTYPE_VARIABLE:
			switch(node.GetVariableType())
			{
				case RAP_VARIABLETYPE_STRING:
				{
					String str;

					str = node.GetVariableValueString();
					str = str.Replace(u8"\"", "\"\""); //" is escaped by putting it double
					textWriter << node.GetName() << " = \"" << str << "\";" << endl;
				}
					break;
				case RAP_VARIABLETYPE_FLOAT:
					textWriter << node.GetName() << " = " << node.GetVariableValueFloat() << ";" << endl;
					break;
				case RAP_VARIABLETYPE_INT:
					textWriter << node.GetName() << " = " << node.GetVariableValueInt() << ";" << endl;
					break;
			}
			break;
		case RAP_PACKETTYPE_ARRAY:
		{
			DynamicArray<RapArrayValue> array = node.ArrayValue();
			textWriter << node.GetName() << "[] = {";
			for(uint32 i = 0; i < array.GetNumberOfElements(); i++)
			{
				WriteRawRapArrayValue(&array[i], textWriter);
				if(i+1 != array.GetNumberOfElements())
				{
					textWriter << ", ";
				}
			}
			textWriter << "};" << endl;
		}
			break;
	}
}

//Namespace Functions
void libBISMod::SaveRawRapTreeToStream(OutputStream &outputStream, const RapTree& rootNode)
{
	TextWriter textWriter(outputStream, TextCodecType::Latin1);

	if(!rootNode.EnumTable().IsEmpty())
	{
		WriteEnumTable(rootNode.EnumTable(), textWriter);
		textWriter << endl << endl;
	}

	for(uint32 i = 0; i < rootNode.ChildNodes().GetNumberOfElements(); i++)
	{
		WriteRawRapPacket(rootNode.ChildNodes()[i], 0, textWriter);
		if(i+1 != rootNode.ChildNodes().GetNumberOfElements())
		{
			textWriter << endl;
		}
	}
}
