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
#include <StdXX.hpp>
#include <libBISMod.hpp>
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::FileSystem;

//Prototypes
void PrintManual();

static void ParseFeedback(const RapParseFeedback& feedback, const String& context, const RapParseContext& parseContext)
{
	String location = u8" at: " + parseContext.filePath + u8":" + String::Number(parseContext.lineNumber);
	switch(feedback)
	{
		case RapParseFeedback::ExtraSemicolon:
			stdErr << u8"Extra semicolon in class '" << context << u8"' detected";
			break;
		case RapParseFeedback::MissingSemicolonAtClassEnd:
			stdErr << u8"Class '" << context << u8"' should be terminated by semicolon";
			break;
		case RapParseFeedback::MissingSemicolonAtPropertyAssignmentEnd:
			stdErr << u8"Assignment to property '" << context << u8"' should be terminated by semicolon. It was guessed by line break (since ArmA CWA engine does so) but it is considered dirty";
			break;
		case RapParseFeedback::UnquotedString:
			stdErr << u8"String '" << context << u8"' should be placed into double quotes for better readability";
			break;
	}

	stdErr << location << endl;
}

static void bin2cpp(const Path& input)
{
	RapTree tree;
	ReadRapTreeFromFile(input, &tree);
	SaveRawRapTreeToStream(stdOut, tree);

	/*
	 * else
	{
		stdOut << "File \"" << g_Input << "\" couldn't be read." << endl;
		return false;
	}
	 */
}

static void cpp2bin(const Path& input)
{
	//RapPreprocessFile(input, stdOut);
	//NOT_IMPLEMENTED_ERROR;

	UniquePointer<RapTree> tree = RapParseFile(input, ParseFeedback);
	SaveRapTreeToStream(stdOut, *tree);
	/*
	else
	{
		stdOut << "Error on line " << ctx.lineNumber << ": " << GetErrorDescription(result) << endl << "Context:" << endl << ctx.filePath << endl << endl;
		return false;
	}*/
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	if(args.GetNumberOfElements() != 1)
	{
		PrintManual();
		return EXIT_FAILURE;
	}

	const auto& osFileSystem = FileSystemsManager::Instance().OSFileSystem();

	Path input = osFileSystem.ToAbsolutePath(osFileSystem.FromNativePath(args[0]));

	if(input.GetFileExtension() == u8"cpp")
		cpp2bin(input);
	else
		bin2cpp(input);

	return EXIT_SUCCESS;
}

void PrintManual()
{
	stdOut << u8"raPEdit" << " by " << u8"Amir Czwink" << endl << endl
		   << "Usage: " << endl
		   << "  raPEdit input" << endl << endl
		   << "   input      either a raw or a raP config file" << endl;
}