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
#include <StdXXMath.hpp>
#include <libBISMod/wrp/World.hpp>

//Definitions
#define WRP_4WVR_SIGNATURE "4WVR"
#define WRP_4WVR_SIGNATURELENGTH 4
#define WRP_4WVR_NUMBEROFTEXTUREFILENAMES 512
#define WRP_4WVR_TEXTUREFILENAMELENGTH 32
#define WRP_4WVR_OBJECT_P3DFILENAMELENGTH 76

class Object_4WVR : public libBISMod::WorldObject
{
public:
	//Members
	StdXX::Math::Matrix4S transformation;
	uint32 id;
	StdXX::String p3dFileName;

	//Methods
	StdXX::String GetModelFilePath() const override;

	void SetModelFilePath(const StdXX::String &modelFilePath) override;
};

class World_4WVR : public libBISMod::World
{
public:
	//Constructor
	World_4WVR(uint32 dimX, uint32 dimY, StdXX::InputStream& inputStream);

	//Methods
	uint32 GetNumberOfObjects() const override;
	libBISMod::WorldObject &GetObject(uint32 index) override;
	void Write(StdXX::OutputStream &outputStream) const override;

private:
	//State
	StdXX::FixedTable<uint16> elevations;
	StdXX::FixedTable<uint16> textureIndices;
	StdXX::String textureFileNames[WRP_4WVR_NUMBEROFTEXTUREFILENAMES];
	StdXX::DynamicArray<StdXX::UniquePointer<Object_4WVR>> objects;
};