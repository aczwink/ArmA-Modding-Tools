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
#include <StdXXFileSystem.hpp>
//Local
#include "Definitions.hpp"
#include "CRapArrayValue.hpp"

namespace libBISMod
{
	//Forward Declarations
	class CRapTree;
	struct SRapErrorContext;

	class CRapNode
	{
		friend void SaveRawRapTreeToFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
	public:
		//Methods
		uint32 AddArrayValue(const CRapArrayValue &refValue);
		uint32 AddNode(const CRapNode &refNode);
		CRapArrayValue &GetArrayValue(uint32 index);
		CRapNode &GetNode(uint32 index);
		void SetInheritedClassName(StdXX::String name);
		void SetName(StdXX::String name);
		void SetPacketType(ERapPacketType type);
		void SetValue(int32 i);
		void SetValue(float32 f);
		void SetValue(StdXX::String str);
		void SetVariableType(ERapVariableType type);

		//Inline
		inline StdXX::String GetInheritedClassName() const
		{
			return this->inheritedClassname;
		}

		inline StdXX::String GetName() const
		{
			return this->name;
		}

		inline uint32 GetNumberOfEmbeddedPackages() const
		{
			return this->embeddedPackets.GetNumberOfElements();
		}

		inline ERapPacketType GetPacketType() const
		{
			return this->packetType;
		}

		inline ERapVariableType GetVariableType() const
		{
			return this->varType;
		}

		inline void GetVariableValueArray(StdXX::DynamicArray<CRapArrayValue> &refArray)
		{
			refArray = this->arrayValues;
		}

		inline float32 GetVariableValueFloat() const
		{
			return this->fValue;
		}

		inline int32 GetVariableValueInt() const
		{
			return this->iValue;
		}

		inline StdXX::String GetVariableValueString() const
		{
			return this->strValue;
		}

	private:
		//State
		StdXX::String name;
		ERapPacketType packetType;
		StdXX::DynamicArray<CRapArrayValue> arrayValues; //Only for array packet type
		StdXX::DynamicArray<CRapNode> embeddedPackets; //Only for class packet type
		StdXX::String inheritedClassname; //Only for class packet type
		ERapVariableType varType; //Only for variable packet type
		int32 iValue; //Only for variable packet type
		float32 fValue; //Only for variable packet type
		StdXX::String strValue; //Only for variable packet type
	};

	class CRapTree : public CRapNode
	{
		friend void RapParse(StdXX::String source, StdXX::String rootName, CRapTree *pRootNode, SRapErrorContext *pCtx);
		friend void ReadRapTreeFromFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
		friend void SaveRapTreeToFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
		friend void SaveRawRapTreeToFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
	private:
		//State
		StdXX::BinaryTreeMap<StdXX::String, uint32> intDefs;
		StdXX::BinaryTreeMap<StdXX::String, float32> floatDefs;
		StdXX::BinaryTreeMap<uint32, StdXX::String> stringDefs;
	};
}