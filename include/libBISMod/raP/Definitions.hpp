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
	enum RapArrayType
	{
		RAP_ARRAYTYPE_STRING,
		RAP_ARRAYTYPE_FLOAT,
		RAP_ARRAYTYPE_INT,
		RAP_ARRAYTYPE_EMBEDDEDARRAY
	};

	enum RapPacketType
	{
		RAP_PACKETTYPE_CLASS,
		RAP_PACKETTYPE_VARIABLE,
		RAP_PACKETTYPE_ARRAY
	};

	enum class RapParseFeedback
	{
		ExtraSemicolon,
		MissingSemicolonAtClassEnd,
		MissingSemicolonAtPropertyAssignmentEnd,
		UnquotedString
	};

	enum RapVariableType
	{
		RAP_VARIABLETYPE_STRING,
		RAP_VARIABLETYPE_FLOAT,
		RAP_VARIABLETYPE_INT
	};

	//Structs
	struct RapParseContext
	{
		StdXX::String filePath;
		uint32 lineNumber;
	};
}