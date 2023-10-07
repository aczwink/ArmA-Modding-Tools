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
#include <libBISMod/p3d/MLOD_SP3X.hpp>
#include <libBISMod/p3d/MLOD_P3DM.hpp>
#include <libBISMod/p3d/ODOL7Lod.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;
using namespace StdXX::CommandLine;
using namespace StdXX::CommonFileFormats;

static void dump_json(const P3DData& p3d)
{
	auto json = JsonValue::Object();
	auto lods = JsonValue::Array();

	for(const auto& lod : p3d.lods)
	{
		auto jsonLod = JsonValue::Object();
		auto polygons = JsonValue::Array();

		for(uint32 i = 0; i < lod->GetNumberOfPolygons(); i++)
		{
			P3DPolygon polygon;
			lod->GetPolygon(i, polygon);

			auto jsonPolygon = JsonValue::Object();
			jsonPolygon[u8"texturePath"] = polygon.texturePath;
			polygons.Push(Move(jsonPolygon));
		}

		jsonLod[u8"polygons"] = Move(polygons);
		lods.Push(Move(jsonLod));
	}

	json[u8"lods"] = Move(lods);

	stdOut << json.Dump() << endl;
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
static void replace_texture(String& texturePath, const BinaryTreeMap<String, String>& textureReplacements)
{
	auto it = textureReplacements.Find(texturePath.ToLowercase());

	if(it != textureReplacements.end())
		texturePath = it.operator*().value;
}
static void replace_resources(P3DData& p3d, const FileSystem::Path& path)
{
	BinaryTreeMap<String, String> textureReplacements;

	FileInputStream fileInputStream(path);
	BufferedInputStream bufferedInputStream(fileInputStream);
	Serialization::JSONDeserializer deserializer(bufferedInputStream);

	deserializer >> textureReplacements;
	ConvertKeysToLowercase(textureReplacements);

	for(auto& lod : p3d.lods)
	{
		switch(lod->GetType())
		{
			case LodType::MLOD_SP3X:
			{
				auto& sp3X_lod = dynamic_cast<MLOD_SP3X_Lod&>(*lod);

				for(auto& polygon : sp3X_lod.lodData.polygons)
					replace_texture(polygon.texturePath, textureReplacements);
			}
			break;
			case LodType::MLOD_P3DM:
			{
				auto& p3dm_lod = dynamic_cast<MLOD_P3DM_Lod&>(*lod);

				for(auto& polygon : p3dm_lod.lodData.polygons)
					replace_texture(polygon.texturePath, textureReplacements);
			}
			break;
			case LodType::ODOL7:
			{
				auto& odol7_lod = dynamic_cast<ODOL7Lod&>(*lod);

				for(auto& texture : odol7_lod.lodData.textures)
					replace_texture(texture, textureReplacements);
			}
			break;
		}
	}

	p3d.Write(stdOut);
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	Parser commandLineParser(programName);
	commandLineParser.AddHelpOption();

	PathArgument inputPathArg(u8"inPath", u8"path to the input file");
	commandLineParser.AddPositionalArgument(inputPathArg);

	SubCommandArgument subCommandArgument(u8"command", u8"The command that should be executed");

	Group json(u8"json", u8"Dump model as JSON");
	subCommandArgument.AddCommand(json);

	Group replace(u8"replace-textures", u8"Replace textures in model");
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
	UniquePointer<P3DData> p3d = ReadP3DFile(fileInputStream);

	if(matchResult.IsActivated(json))
		dump_json(*p3d);
	else if(matchResult.IsActivated(replace))
		replace_resources(*p3d, replaceFileArg.Value(matchResult));

	return EXIT_SUCCESS;
}