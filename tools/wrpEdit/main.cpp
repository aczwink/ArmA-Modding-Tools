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
#include <libBISMod.hpp>
#include <StdXX.hpp>
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::CommandLine;
using namespace StdXX::CommonFileFormats;

static void dump_json(const World& world)
{
	auto json = JsonValue::Object();
	auto objects = JsonValue::Array();

	for(uint32 i = 0; i < world.GetNumberOfObjects(); i++)
	{
		auto jsonObject = JsonValue::Object();

		jsonObject[u8"modelFilePath"] = world.GetObject(i).GetModelFilePath();

		objects.Push(Move(jsonObject));
	}

	json[u8"objects"] = Move(objects);

	stdOut << json.Dump() << endl;
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	Parser commandLineParser(programName);
	commandLineParser.AddHelpOption();

	PathArgument inputPathArg(u8"inPath", u8"path to the input file");
	commandLineParser.AddPositionalArgument(inputPathArg);

	SubCommandArgument subCommandArgument(u8"command", u8"The command that should be executed");

	Group json(u8"json", u8"Dump world as JSON");
	subCommandArgument.AddCommand(json);

	commandLineParser.AddPositionalArgument(subCommandArgument);

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
	FileInputStream fileInputStream(inputPath);
	UniquePointer<World> world = LoadWorld(fileInputStream);

	if(matchResult.IsActivated(json))
		dump_json(*world);

	return EXIT_SUCCESS;
}