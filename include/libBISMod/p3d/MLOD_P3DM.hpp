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
//Local
#include "P3DLod.hpp"
#include "MLOD_SP3X.hpp"
//Definitions
#define P3D_LODHEADER_P3DM_SIGNATURE "P3DM"
#define P3D_LODHEADER_P3DM_VERSIONMAJOR 0x1C
#define P3D_LODHEADER_P3DM_VERSIONMINOR 0x100

namespace libBISMod
{
	struct SP3DMTag
	{
		bool active;
		char *pTagName; //zero-terminated
		uint32 dataSize;
		byte *pData; //'dataSize' number of bytes
	};

	struct SP3DMLodData
	{
		uint32 nVertices;
		uint32 nNormals;
		uint32 unknownFlags;
		SVertex *pVertices;
		StdXX::Math::Vector3S *pNormals;
		StdXX::DynamicArray<MLOD_Polygon> polygons;
		char tagSignature[4];
		SP3DMTag *pTags;
		float resolution;
	};

	class MLOD_P3DM_Lod : public P3DLod
	{
	public:
		//State
		SP3DMLodData lodData;

		//Constructor
		inline MLOD_P3DM_Lod(StdXX::InputStream& inputStream)
		{
			this->Read(inputStream);
		}

		//Destructor
		~MLOD_P3DM_Lod();

		//Methods
		uint32 GetNumberOfPolygons() const override;
		void GetPolygon(uint32 index, P3DPolygon &polygon) const override;
		LodType GetType() const override;
		void Write(StdXX::OutputStream &outputStream) const override;

	private:
		//State
		uint32 nTags;

		//Methods
		void Read(StdXX::InputStream& inputStream);
		void ReadTags(StdXX::InputStream& inputStream);
	};
}