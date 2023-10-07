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
		//Methods
		void SetType(ERapArrayType type);
		void SetValue(int32 i);
		void SetValue(float32 f);
		void SetValue(StdXX::String str);
		void SetValue(const StdXX::DynamicArray<RapArrayValue> &refArray);

		//Inline
		inline ERapArrayType GetType() const
		{
			return this->type;
		}

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

		inline StdXX::String GetValueString() const
		{
			return this->str;
		}

	private:
		//Variables
		ERapArrayType type;
		float32 fValue;
		int32 iValue;
		StdXX::String str;
		StdXX::DynamicArray<RapArrayValue> embeddedArray;
	};
}