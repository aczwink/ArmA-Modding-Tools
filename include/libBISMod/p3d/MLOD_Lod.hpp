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
//Local
#include "P3DLod.hpp"
#include "MLOD.hpp"

namespace libBISMod
{
	class MLOD_Lod : public P3DLod
	{
	public:
		//State
		uint32 unknownFlags;
		StdXX::DynamicArray<MLOD_Vertex> vertices;
		StdXX::DynamicArray<StdXX::Math::Vector3S> faceNormals;
		StdXX::DynamicArray<MLOD_Face> faces;
		StdXX::DynamicArray<MLOD_Tag> tags;
		float32 resolution;

		//Constructors
		inline MLOD_Lod(bool isSP3X) : isSP3X(isSP3X)
		{
		}

		inline MLOD_Lod(StdXX::InputStream& inputStream)
		{
			this->Read(inputStream);
		}

		//Methods
		uint32 GetNumberOfPolygons() const override;
		void GetPolygon(uint32 index, P3DPolygon &polygon) const override;
		LodType GetType() const override;
		void Write(StdXX::OutputStream &outputStream) const override;

	private:
		//State
		bool isSP3X;

		//Methods
		void Read(StdXX::InputStream& inputStream);
		void ReadTags(StdXX::InputStream& inputStream);
		void WriteTag(const MLOD_Tag& tag, StdXX::DataWriter& dataWriter) const;

		//Inline
		inline void ReadVec3(StdXX::Math::Vector3S& vec3, StdXX::DataReader& dataReader)
		{
			vec3.x = dataReader.ReadFloat32();
			vec3.y = dataReader.ReadFloat32();
			vec3.z = dataReader.ReadFloat32();
		}

		inline void WriteVec3(const StdXX::Math::Vector3S& vec3, StdXX::DataWriter& dataWriter) const
		{
			dataWriter.WriteFloat32(vec3.x);
			dataWriter.WriteFloat32(vec3.y);
			dataWriter.WriteFloat32(vec3.z);
		}
	};
}