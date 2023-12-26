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
#include "RapNode.hpp"

namespace libBISMod
{
	StdXX::UniquePointer<RapTree> RapParseFile(const StdXX::FileSystem::Path& inputPath, const StdXX::Function<void(const RapParseFeedback&, const StdXX::String&, const RapParseContext& context)>& parseFeedback);
	void RapPreprocessFile(const StdXX::FileSystem::Path& inputPath, StdXX::TextWriter& textWriter);
	StdXX::UniquePointer<RapTree> ReadRapTreeFromFile(const StdXX::FileSystem::Path& path);
	void ValidateRapTree(const RapTree& tree);
	void SaveRapTreeToStream(StdXX::OutputStream &outputStream, const RapTree& rapTree);
	void SaveRawRapTreeToStream(StdXX::OutputStream &outputStream, const RapTree& rapTree);
}