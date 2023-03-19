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
#include <libBISMod/raP/CRapNode.hpp>

//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;
//Definitions
#define PUTTABS for(uint32 i = 0; i < nTabs; i++){ textWriter << '\t'; }

//Local functions
static void WriteRawRapArrayValue(CRapArrayValue *pValue, FileOutputStream &file)
{
	TextWriter textWriter(file, TextCodecType::ASCII);

	switch(pValue->GetType())
	{
		case RAP_ARRAYTYPE_STRING:
			textWriter << '"' << pValue->GetValueString() << '"';
			break;
		case RAP_ARRAYTYPE_FLOAT:
			textWriter << pValue->GetValueFloat();
			break;
		case RAP_ARRAYTYPE_INT:
			textWriter << pValue->GetValueInt();
			break;
		case RAP_ARRAYTYPE_EMBEDDEDARRAY:
		{
			DynamicArray<CRapArrayValue> array;

			pValue->GetValueArray(array);
			textWriter << '{';
			for(uint32 i = 0; i < array.GetNumberOfElements(); i++)
			{
				WriteRawRapArrayValue(&array[i], file);
				if(i+1 != array.GetNumberOfElements())
				{
					textWriter << ", ";
				}
			}
			textWriter << '}';
		}
			break;
	}
}

static void WriteRawRapPacket(CRapNode *pNode, uint16 nTabs, FileOutputStream &file)
{
	TextWriter textWriter(file, TextCodecType::ASCII);
	PUTTABS;

	switch(pNode->GetPacketType())
	{
		case RAP_PACKETTYPE_CLASS:
			textWriter << "class " << pNode->GetName();
			if(!pNode->GetInheritedClassName().IsEmpty())
			{
				textWriter << " : " << pNode->GetInheritedClassName();
			}
			textWriter << endl;
			PUTTABS;
			textWriter << '{' << endl;
			for(uint32 i = 0; i < pNode->GetNumberOfEmbeddedPackages(); i++)
			{
				WriteRawRapPacket(&pNode->GetNode(i), nTabs+1, file);
				if(i+1 != pNode->GetNumberOfEmbeddedPackages())
				{
					if(pNode->GetNode(i+1).GetPacketType() == RAP_PACKETTYPE_CLASS)
					{
						textWriter << endl;
					}
				}
			}
			PUTTABS;
			textWriter << "};" << endl;
			break;
		case RAP_PACKETTYPE_VARIABLE:
			switch(pNode->GetVariableType())
			{
				case RAP_VARIABLETYPE_STRING:
				{
					String str;

					str = pNode->GetVariableValueString();
					str.Replace(u8"\"", "\"\""); //" is escaped by putting it double
					textWriter << pNode->GetName() << " = \"" << str << "\";" << endl;
				}
					break;
				case RAP_VARIABLETYPE_FLOAT:
					textWriter << pNode->GetName() << " = " << pNode->GetVariableValueFloat() << ";" << endl;
					break;
				case RAP_VARIABLETYPE_INT:
					textWriter << pNode->GetName() << " = " << pNode->GetVariableValueInt() << ";" << endl;
					break;
			}
			break;
		case RAP_PACKETTYPE_ARRAY:
		{
			DynamicArray<CRapArrayValue> array;

			pNode->GetVariableValueArray(array);
			textWriter << pNode->GetName() << "[] = {";
			for(uint32 i = 0; i < array.GetNumberOfElements(); i++)
			{
				WriteRawRapArrayValue(&array[i], file);
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
void libBISMod::SaveRawRapTreeToFile(const Path& path, CRapTree *pRootNode)
{
	FileOutputStream file(path);
	TextWriter textWriter(file, TextCodecType::ASCII);

	if(pRootNode->intDefs.GetNumberOfElements() || pRootNode->floatDefs.GetNumberOfElements())
	{
		textWriter << "//Definitions" << endl;
		for(const auto& it : pRootNode->intDefs)
		{
			textWriter << "#define " << it.key << ' ' << it.value << endl;
		}
		for(const auto& it : pRootNode->floatDefs)
		{
			textWriter << "#define " << it.key << ' ' << it.value << endl;
		}
		textWriter << endl << endl;
	}

	for(uint32 i = 0; i < pRootNode->embeddedPackets.GetNumberOfElements(); i++)
	{
		WriteRawRapPacket(&pRootNode->embeddedPackets[i], 0, file);
		if(i+1 != pRootNode->embeddedPackets.GetNumberOfElements())
		{
			textWriter << endl;
		}
	}
}
