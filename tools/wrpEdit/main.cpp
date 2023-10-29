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
//Namespaces
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
		
		const auto& obj = world.GetObject(i);

		jsonObject[u8"id"] = obj.GetId();
		jsonObject[u8"modelFilePath"] = obj.GetModelFilePath();

		objects.Push(Move(jsonObject));
	}

	json[u8"objects"] = Move(objects);

	stdOut << json.Dump() << endl;
}

struct Replacements
{
	BinaryTreeMap<String, String> models;
	BinaryTreeMap<uint32, uint32> ids;
};
namespace StdXX::Serialization
{
	void Archive(Serialization::JSONDeserializer& deserializer, Replacements& replacements)
	{
		deserializer & Serialization::Binding(u8"models", replacements.models);
		deserializer & Serialization::Binding(u8"ids", replacements.ids);
	}
}
static void ConvertKeysToLowercase(BinaryTreeMap<String, String>& mapping)
{
	BinaryTreeMap<String, String> copy;
	for(const auto& entry : mapping)
	{
		copy.Insert(entry.key.ToLowercase(), entry.value);
	}
	mapping = Move(copy);
}
static void replace_resources(World& world, const FileSystem::Path& path)
{
	Replacements replacements;

	FileInputStream fileInputStream(path);
	BufferedInputStream bufferedInputStream(fileInputStream);
	Serialization::JSONDeserializer deserializer(bufferedInputStream);

	deserializer >> replacements;
	ConvertKeysToLowercase(replacements.models);

	for(uint32 i = 0; i < world.GetNumberOfObjects(); i++)
	{
		auto& obj = world.GetObject(i);
		auto it = replacements.models.Find(obj.GetModelFilePath().ToLowercase());

		if(it != replacements.models.end())
		{
			obj.SetModelFilePath(it.operator*().value);
		}

		auto it2 = replacements.ids.Find(obj.GetId());
		if(it2 != replacements.ids.end())
		{
			obj.SetId(it2->value);
		}
	}

	world.Write(stdOut);
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

	Group replace(u8"replace-resources", u8"Replace resources file");
	PathArgument replaceFileArg(u8"replacementFile", u8"path to the replacement JSON file");
	replace.AddPositionalArgument(replaceFileArg);
	subCommandArgument.AddCommand(replace);

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
	else if(matchResult.IsActivated(replace))
		replace_resources(*world, replaceFileArg.Value(matchResult));

	return EXIT_SUCCESS;
}