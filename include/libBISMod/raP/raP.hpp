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
#include <StdXXFileSystem.hpp>
#include "CRapNode.hpp"

namespace libBISMod
{
	void RapParse(StdXX::String source, StdXX::String rootName, CRapTree *pRootNode, SRapErrorContext *pCtx);
	void RapParseFile(const StdXX::FileSystem::Path& inputPath, CRapTree *pRootNode, SRapErrorContext *pCtx);
	void ReadRapTreeFromFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
	void SaveRapTreeToFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
	void SaveRawRapTreeToFile(const StdXX::FileSystem::Path& path, CRapTree *pRootNode);
}