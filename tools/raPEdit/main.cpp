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
using namespace StdXX::CommandLine;
using namespace StdXX::FileSystem;

static void inline_enums(DynamicArray<RapArrayValue>& array, const RapTree& tree)
{
	for(auto& arrayValue : array)
	{
		switch(arrayValue.Type())
		{
			case RAP_ARRAYTYPE_STRING:
				if(tree.IsEnumDefined(arrayValue.ValueString()))
					arrayValue.SetValue((int32)tree.ResolveEnum(arrayValue.ValueString()));
				break;
			case RAP_ARRAYTYPE_EMBEDDEDARRAY:
				inline_enums(arrayValue.ValueArray(), tree);
				break;
		}
	}
}

static void inline_enums(RapNode& rapNode, const RapTree& tree)
{
	switch(rapNode.PacketType())
	{
		case RAP_PACKETTYPE_CLASS:
			for(uint32 i = 0; i < rapNode.GetNumberOfEmbeddedPackages(); i++)
				inline_enums(rapNode.GetNode(i), tree);
			break;
		case RAP_PACKETTYPE_VARIABLE:
		{
			if((rapNode.VariableType() == RapVariableType::RAP_VARIABLETYPE_STRING) and tree.IsEnumDefined(rapNode.VariableValueString()))
				rapNode.SetValue((int32)tree.ResolveEnum(rapNode.VariableValueString()));
		}
		break;
		case RAP_PACKETTYPE_ARRAY:
			inline_enums(rapNode.ArrayValue(), tree);
		break;
	}
}

static void inline_enums(RapTree& tree)
{
	inline_enums(*tree.rootNode, tree);
	tree.ClearEnums();
}

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
	UniquePointer<RapTree> tree = ReadRapTreeFromFile(input);
	SaveRawRapTreeToStream(stdOut, *tree);

	/*
	 * else
	{
		stdOut << "File \"" << g_Input << "\" couldn't be read." << endl;
		return false;
	}
	 */
}

static void cpp2bin(const Path& input, bool inlineEnums)
{
	UniquePointer<RapTree> tree = RapParseFile(input, ParseFeedback);
	ValidateRapTree(*tree);

	if(inlineEnums)
		inline_enums(*tree);

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
	Parser commandLineParser(programName);
	commandLineParser.AddHelpOption();

	Option inlineEnumsOption(u8'i', u8"inline-enums", u8"Replace enum references with their enum value and don't write an enum table. Ignored when a binarized raP file is passed.");
	commandLineParser.AddOption(inlineEnumsOption);

	Option preprocessOption(u8'p', u8"preprocess-only", u8"Only run the input through the preprocessor but do not parse. Ignored when a binarized raP file is passed.");
	commandLineParser.AddOption(preprocessOption);

	PathArgument inputPathArg(u8"inPath", u8"path to the input file");
	commandLineParser.AddPositionalArgument(inputPathArg);

	if(!commandLineParser.Parse(args))
	{
		stdErr << commandLineParser.GetErrorText() << endl;
		return EXIT_FAILURE;
	}
	if(commandLineParser.IsHelpActivated())
	{
		commandLineParser.PrintHelp();
		return EXIT_SUCCESS;
	}

	const MatchResult& matchResult = commandLineParser.ParseResult();
	FileSystem::Path inputPath = inputPathArg.Value(matchResult);

	if(inputPath.GetFileExtension() == u8"bin")
		bin2cpp(inputPath);
	else if(matchResult.IsActivated(preprocessOption))
		RapPreprocessFile(inputPath, stdOut);
	else
		cpp2bin(inputPath, matchResult.IsActivated(inlineEnumsOption));

	return EXIT_SUCCESS;
}