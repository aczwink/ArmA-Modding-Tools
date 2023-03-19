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
#include <StdXXMath.hpp>
#include "P3DModelInfo.hpp"

namespace libBISMod
{
	struct SODOL7ModelInfo
	{
		StdXX::DynamicArray<float32> resolutions;
		uint32 index;
		float memLodSphere;
		float geoLodSphere;
		uint32 vertexFlags[3];
		StdXX::Math::Vector3S offset1;
		uint32 mapIconColor;
		uint32 mapSelectedColor;
		float viewDensity;
		StdXX::Math::Vector3S bBoxMinPosition;
		StdXX::Math::Vector3S bBoxMaxPosition;
		StdXX::Math::Vector3S centreOfGravity;
		StdXX::Math::Vector3S offset2;
		StdXX::Math::Vector3S cogOffset;
		StdXX::Math::Vector3S modelMassVectors[3];
		bool autoCenter;
		bool lockAutoCenter;
		bool canOcclude;
		bool canBeOccluded;
		bool allowAnimation;
		byte unknown;
		uint32 nUknownFloats;
		float *pUnknownFloats; //Potentially compressed
		float mass;
		float massReciprocal;
		float altMass;
		float altMassReciprocal;
		byte unknownByteIndices[12];
	};

	class ODOL7ModelInfo : public P3DModelInfo
	{
	public:
		//State
		SODOL7ModelInfo modelInfo;

		//Constructor
		inline ODOL7ModelInfo(StdXX::InputStream& inputStream, uint32 nLods)
		{
			this->Read(inputStream, nLods);
		}

		//Destructor
		~ODOL7ModelInfo();

		//Methods
		void Write(StdXX::OutputStream &outputStream) const override;

	private:
		//Methods
		void Read(StdXX::InputStream& inputStream, uint32 nLods);

		//Inline
		inline bool ReadBool(StdXX::DataReader& dataReader)
		{
			return dataReader.ReadByte() != 0;
		}
	};
}