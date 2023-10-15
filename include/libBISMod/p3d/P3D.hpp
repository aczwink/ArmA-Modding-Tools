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
#include <StdXXFileSystem.hpp>
#include "P3DLod.hpp"
#include "P3DModelInfo.hpp"

namespace libBISMod
{
	enum class P3DType
	{
		MLOD,
		ODOL7
	};

	class P3DData
	{
	public:
		//State
		StdXX::DynamicArray<StdXX::UniquePointer<P3DLod>> lods;
		StdXX::UniquePointer<P3DModelInfo> modelInfo;

		//Constructor
		inline P3DData(P3DType type) : type(type)
		{
		}

		//Properties
		inline P3DType Type() const
		{
			return this->type;
		}

		//Methods
		StdXX::UniquePointer<P3DData> Convert(LodType targetLodType) const;
		void Write(StdXX::OutputStream& outputStream) const;

	private:
		//State
		P3DType type;

		//Methods
		StdXX::UniquePointer<P3DLod> ConvertLod(uint32 lodIndex, LodType targetType) const;
		StdXX::UniquePointer<P3DLod> ConvertODOL7LodToMLOD(const P3DLod& sourceLod, LodType targetType, uint32 lodIndex) const;
		P3DType MapLodTypeToP3DType(LodType lodType) const;
	};

	StdXX::String LodResolutionToString(float resolutiuon);
	StdXX::String P3dTypeToString(P3DType type);
	StdXX::UniquePointer<P3DData> ReadP3DFile(StdXX::InputStream& inputStream);
}