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

void bin2cpp(const Path& input, const Path& output)
{
	CRapTree tree;
	ReadRapTreeFromFile(input, &tree);
	SaveRawRapTreeToFile(output, &tree);

	/*
	 * else
	{
		stdOut << "File \"" << g_Input << "\" couldn't be read." << endl;
		return false;
	}
	 */
}

void cpp2bin(const Path& input, const Path& output)
{
	CRapTree tree;
	SRapErrorContext ctx;

	RapParseFile(input, &tree, &ctx);
	SaveRapTreeToFile(output, &tree);
	/*
	else
	{
		stdOut << "Error on line " << ctx.lineNumber << ": " << GetErrorDescription(result) << endl << "Context:" << endl << ctx.context << endl << endl;
		return false;
	}*/
}

int32 Main(const String &programName, const FixedArray<String> &args)
{
	if(args.GetNumberOfElements() != 2)
	{
		PrintManual();
		return EXIT_FAILURE;
	}

	const auto& osFileSystem = FileSystemsManager::Instance().OSFileSystem();

	Path input = osFileSystem.FromNativePath(args[0]);
	Path output = osFileSystem.FromNativePath(args[1]);

	if(input.GetFileExtension() == u8"cpp")
		cpp2bin(input, output);
	else
		bin2cpp(input, output);

	return EXIT_SUCCESS;
}

void PrintManual()
{
	stdOut << u8"raPEdit" << " by " << u8"Amir Czwink" << endl << endl
		   << "Usage: " << endl
		   << "  raPEdit input output" << endl << endl
		   << "   input      either a raw or a raP config file" << endl
		   << "   output     file to be created" << endl;
}