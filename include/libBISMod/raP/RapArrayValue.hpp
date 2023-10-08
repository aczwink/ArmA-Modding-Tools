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
#include <StdXXCore.hpp>
//Local
#include "Definitions.hpp"

namespace libBISMod
{
	class RapArrayValue
	{
	public:
		//Properties
		inline RapArrayType Type() const
		{
			return this->type;
		}

		inline StdXX::DynamicArray<RapArrayValue>& ValueArray()
		{
			ASSERT_EQUALS(RapArrayType::RAP_ARRAYTYPE_EMBEDDEDARRAY, this->type);
			return this->embeddedArray;
		}

		inline const StdXX::String& ValueString() const
		{
			ASSERT_EQUALS(RapArrayType::RAP_ARRAYTYPE_STRING, this->type);
			return this->str;
		}

		//Inline
		inline void GetValueArray(StdXX::DynamicArray<RapArrayValue> &refArray) const
		{
			refArray = this->embeddedArray;
		}

		inline float32 GetValueFloat() const
		{
			return this->fValue;
		}

		inline int32 GetValueInt() const
		{
			return this->iValue;
		}

		inline void SetValue(int32 i)
		{
			this->type = RAP_ARRAYTYPE_INT;
			this->iValue = i;
		}

		inline void SetValue(float32 f)
		{
			this->type = RAP_ARRAYTYPE_FLOAT;
			this->fValue = f;
		}

		inline void SetValue(StdXX::String str)
		{
			this->type = RAP_ARRAYTYPE_STRING;
			this->str = str;
		}

		inline void SetValue(StdXX::DynamicArray<RapArrayValue>&& array)
		{
			this->type = RAP_ARRAYTYPE_EMBEDDEDARRAY;
			this->embeddedArray = Move(array);
		}

	private:
		//Variables
		RapArrayType type;
		float32 fValue;
		int32 iValue;
		StdXX::String str;
		StdXX::DynamicArray<RapArrayValue> embeddedArray;
	};
}