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
#include "RapArrayValue.hpp"

namespace libBISMod
{
	//Forward Declarations
	class RapTree;
	struct RapParseContext;

	class RapNode
	{
	public:
		//Constructor
		inline RapNode()
		{
			this->parent = nullptr;
		}

		//Properties
		inline StdXX::DynamicArray<RapArrayValue>& ArrayValue()
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_ARRAY, this->packetType);
			return this->arrayValues;
		}

		inline const StdXX::DynamicArray<RapArrayValue>& ArrayValue() const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_ARRAY, this->packetType);
			return this->arrayValues;
		}

		inline const StdXX::DynamicArray<StdXX::UniquePointer<RapNode>>& ChildNodes() const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_CLASS, this->packetType);
			return this->classData.children;
		}

		inline const StdXX::String& InheritedClassName() const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_CLASS, this->packetType);
			return this->inheritedClassname;
		}

		inline void InheritedClassName(const StdXX::String& newValue)
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_CLASS, this->packetType);
			this->inheritedClassname = newValue;
		}

		inline bool IsDerived() const
		{
			return !this->inheritedClassname.IsEmpty();
		}

		inline RapPacketType PacketType() const
		{
			return this->packetType;
		}

		inline const RapNode* Parent() const
		{
			return this->parent;
		}

		inline const StdXX::String& VariableValueString() const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_VARIABLE, this->packetType);
			ASSERT_EQUALS(RapVariableType::RAP_VARIABLETYPE_STRING, this->varType);
			return this->strValue;
		}

		inline RapVariableType VariableType() const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_VARIABLE, this->packetType);
			return this->varType;
		}

		//Methods
		uint32 AddArrayValue(const RapArrayValue &refValue);
		RapArrayValue &GetArrayValue(uint32 index);
		RapNode &GetNode(uint32 index);
		void SetName(StdXX::String name);
		void SetPacketType(RapPacketType type);

		//Inline
		inline uint32 AddChild(StdXX::UniquePointer<RapNode>&& child)
		{
			child->parent = this;
			return this->classData.children.Push(StdXX::Move(child));
		}

		inline const RapNode& GetChildNode(uint32 index) const
		{
			ASSERT_EQUALS(RapPacketType::RAP_PACKETTYPE_CLASS, this->packetType);
			return *this->classData.children[index];
		}

		inline StdXX::String GetName() const
		{
			return this->name;
		}

		inline uint32 GetNumberOfEmbeddedPackages() const
		{
			return this->classData.children.GetNumberOfElements();
		}

		inline float32 GetVariableValueFloat() const
		{
			return this->fValue;
		}

		inline int32 GetVariableValueInt() const
		{
			return this->iValue;
		}

		inline StdXX::String GlobalName() const
		{
			if(this->parent == nullptr)
				return this->name;
			return this->parent->GlobalName() + u8"/" + this->name;
		}

		inline void SetValue(int32 i)
		{
			this->varType = RAP_VARIABLETYPE_INT;
			this->iValue = i;
		}

		inline void SetValue(float32 f)
		{
			this->varType = RAP_VARIABLETYPE_FLOAT;
			this->fValue = f;
		}

		inline void SetValue(StdXX::String str)
		{
			this->varType = RAP_VARIABLETYPE_STRING;
			this->strValue = str;
		}

	private:
		//State
		StdXX::String name;
		RapPacketType packetType;
		RapNode* parent;
		struct
		{
			StdXX::DynamicArray<StdXX::UniquePointer<RapNode>> children;
		} classData;
		StdXX::DynamicArray<RapArrayValue> arrayValues; //Only for array packet type
		StdXX::String inheritedClassname; //Only for class packet type
		RapVariableType varType; //Only for variable packet type
		int32 iValue; //Only for variable packet type
		float32 fValue; //Only for variable packet type
		StdXX::String strValue; //Only for variable packet type
	};

	class RapTree
	{
	public:
		//State
		StdXX::UniquePointer<RapNode> rootNode;

		//Properties
		inline const StdXX::BinaryTreeMap<StdXX::String, uint32>& EnumTable() const
		{
			return this->enumTable;
		}

		//Inline
		inline void ClearEnums()
		{
			this->enumCasingTable.Release();
			this->enumTable.Release();
		}

		inline void DefineEnumValue(const StdXX::String& enumName, uint32 value)
		{
			this->enumTable[enumName] = value;
			this->enumCasingTable[enumName.ToLowercase()] = enumName;
		}

		inline bool IsEnumDefined(const StdXX::String& enumName) const
		{
			return this->enumCasingTable.Contains(enumName.ToLowercase());
		}

		inline uint32 ResolveEnum(const StdXX::String& enumName) const
		{
			auto correctCasing = this->enumCasingTable.Get(enumName.ToLowercase());
			return this->enumTable.Get(correctCasing);
		}

	private:
		//State
		StdXX::BinaryTreeMap<StdXX::String, uint32> enumTable;
		StdXX::BinaryTreeMap<StdXX::String, StdXX::String> enumCasingTable;
	};
}