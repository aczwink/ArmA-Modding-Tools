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
using namespace StdXX;

//Definitions
#define P3D_GAPSIZE_COMPRESSION 1023 //Array which is bigger than this is compressed

void LZSSCompressData(const byte *pInput, uint32 uncompressedSize, byte *pOutput, uint32 &refOutputSize);
bool LZSSReadCompressed(byte *pOutput, uint32 uncompressedSize, InputStream &refInput);