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
#include <libBISMod/wrp/World.hpp>
//Libs
#include <StdXXStreams.hpp>
//Local
#include "World_4WVR.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;

//Namespace functions
World *libBISMod::LoadWorld(SeekableInputStream& inputStream)
{
	DataReader dataReader(false, inputStream);

	//read header
	byte signature[WRP_4WVR_SIGNATURELENGTH];
	uint32 dimX, dimY;

	dataReader.ReadBytes(signature, sizeof(signature));

	dimX = dataReader.ReadUInt32();
	dimY = dataReader.ReadUInt32();

	if(MemCmp(signature, WRP_4WVR_SIGNATURE, WRP_4WVR_SIGNATURELENGTH) == 0)
		return new World_4WVR(dimX, dimY, inputStream);

	ASSERT_EQUALS(0, inputStream.QueryRemainingBytes());

	return nullptr;
}
