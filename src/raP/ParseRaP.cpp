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
#include <libBISMod/raP/raP.hpp>
//Libs
#include <StdXX.hpp>
//Local
#include <libBISMod/raP/RapNode.hpp>
#include "RapLexer.hpp"
#include "raPInternal.hpp"
#include "RapParser.hpp"
#include "RapPreprocessor.hpp"
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;

//Namespace functions
UniquePointer<RapTree> libBISMod::RapParseFile(const Path &inputPath, const Function<void(const RapParseFeedback&, const String&, const RapParseContext& context)>& parseFeedback)
{
	RapPreprocessor rapPreprocessor(inputPath.GetParent());
	rapPreprocessor.PushSource(inputPath.GetName());

	RapLexer lexer(rapPreprocessor);
	RapParser parser(lexer, inputPath.GetName(), parseFeedback);
	parser.Parse();

	auto tree = parser.TakeRoot();
	return tree;
}

void libBISMod::RapPreprocessFile(const Path& inputPath, TextWriter& textWriter)
{
	RapPreprocessor rapPreprocessor(inputPath.GetParent());
	rapPreprocessor.PushSource(inputPath.GetName());

	uint32 c;
	while((c = rapPreprocessor.ReadNextCodePoint()) != 0)
	{
		textWriter.WriteCodePoint(c);
	}
}