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
//Class header
#include <libBISMod/raP/CRapNode.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;

//Public methods
uint32 CRapNode::AddArrayValue(const CRapArrayValue &refValue)
{
	return this->arrayValues.Push(refValue);
}

uint32 CRapNode::AddNode(const CRapNode &refNode)
{
	return this->embeddedPackets.Push(refNode);
}

CRapArrayValue &CRapNode::GetArrayValue(uint32 index)
{
	return this->arrayValues[index];
}

CRapNode &CRapNode::GetNode(uint32 index)
{
	return this->embeddedPackets[index];
}

void CRapNode::SetInheritedClassName(String name)
{
	this->inheritedClassname = name;
}

void CRapNode::SetName(String name)
{
	this->name = name;
}

void CRapNode::SetPacketType(ERapPacketType type)
{
	this->packetType = type;
}

void CRapNode::SetValue(int32 i)
{
	this->iValue = i;
}

void CRapNode::SetValue(float32 f)
{
	this->fValue = f;
}

void CRapNode::SetValue(String str)
{
	this->strValue = str;
}

void CRapNode::SetVariableType(ERapVariableType type)
{
	this->varType = type;
}