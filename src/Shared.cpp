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
//Corresponding header
#include "Shared.hpp"

Matrix4S ReadTransformation(InputStream& inputStream)
{
	Matrix4S tmp;

	DataReader dataReader(false, inputStream);

	tmp(3, 3) = 1;
	for(uint8 y = 0; y < 4; y++)
	{
		for(uint8 x = 0; x < 3; x++)
		{
			tmp(y, x) = dataReader.ReadFloat32();
		}
	}

	return tmp;
}

void WriteTransformation(const Matrix4S &transformation, DataWriter &dataWriter)
{
	for(uint8 y = 0; y < 4; y++)
	{
		for(uint8 x = 0; x < 3; x++)
		{
			dataWriter.WriteFloat32(transformation(y, x));
		}
	}
}
