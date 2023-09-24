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

namespace libBISMod
{
	class WorldObject
	{
	public:
		//Destructor
		virtual ~WorldObject() = default;

		//Methods
		virtual StdXX::String GetModelFilePath() const = 0;
	};

	class World
	{
	public:
		//Destructor
		virtual ~World() = default;

		//Methods
		virtual uint32 GetNumberOfObjects() const = 0;
		virtual const WorldObject& GetObject(uint32 index) const = 0;
		virtual void Write(StdXX::OutputStream& outputStream) const = 0;
	};

	World* LoadWorld(StdXX::SeekableInputStream& inputStream);
}