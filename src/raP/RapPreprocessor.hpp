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
#pragma once
#include <StdXX.hpp>
#include <libBISMod/raP/Definitions.hpp>
#include "ContextAwareCharStreamReader.hpp"
using namespace StdXX;
using namespace StdXX::FileSystem;

class RapPreprocessor : public ContextAwareCharStreamReader
{
	struct SourceInfo
	{
		UniquePointer<InputStream> sourceStream;
		Optional<libBISMod::RapParseContext> context;
		TextCodecType textCodecType;
		BinaryTreeMap<String, String> macroVarAssignments;

		//Constructors
		inline SourceInfo(UniquePointer<InputStream>&& sourceStream) : sourceStream(Move(sourceStream)), textCodecType(TextCodecType::UTF8)
		{
		}

		inline SourceInfo(UniquePointer<InputStream>&& sourceStream, Optional<libBISMod::RapParseContext>&& context)
			: sourceStream(Move(sourceStream)), context(Move(context)), textCodecType(TextCodecType::Latin1)
		{
		}
	};
public:
	//Constructor
	inline RapPreprocessor(const Path& rootPath) : rootPath(rootPath)
	{
	}

	//Methods
	const libBISMod::RapParseContext &QueryCurrentContext() const override;
	uint32 ReadNextCodePoint() override;

	//Inline
	inline void PushSource(const Path& relativePath)
	{
		ChainedInputStream* chainedInputStream = new ChainedInputStream(new FileInputStream(this->rootPath / relativePath.String().Replace(u8"\\", u8"/")));
		chainedInputStream->Add(new BufferedInputStream(chainedInputStream->GetEnd()));
		libBISMod::RapParseContext ctx{relativePath.String(), 1};
		this->sourceStack.InsertTail(SourceInfo(chainedInputStream, Move(ctx)));
	}

private:
	//State
	Path rootPath;
	LinkedList<SourceInfo> sourceStack;
	BinaryTreeMap<String, String> definitions;
	BinaryTreeMap<String, Tuple<DynamicArray<String>, String>> macros;
	DynamicArray<bool> branches;
	LinkedList<uint32> buffer;

	//Properties
	inline libBISMod::RapParseContext& CurrentContextEditable()
	{
		return const_cast<libBISMod::RapParseContext &>(this->QueryCurrentContext());
	}

	//Methods
	String Expand(const String& value) const;
	uint32 HandleDefine();
	uint32 HandlePreprocessorDirective();
	uint32 HandleSlash();
	bool IsDelimiterSymbol(uint32 codePoint) const;
	String ReadDefinitionValue(uint32& codePoint);
	uint32 ReadNextCodePointFromCurrentStream();
	String ReadPreprocessorDirective(uint32& codePoint);
	uint32 SkipBranch();
	bool TryResolveVariable(const String &identifier, String& expanded) const;

	//Inline
	inline void IncreaseLineCounter()
	{
		this->CurrentContextEditable().lineNumber++;
	}

	inline void PushCodePointSource(uint32 codePoint)
	{
		String tmp;
		tmp += codePoint;
		this->PushDirectSource(tmp);
	}

	inline void PushDirectSource(const String& source)
	{
		this->sourceStack.InsertTail(SourceInfo(new StringInputStream(source, true)));
	}

	inline void PutIntoBuffer(const String& string)
	{
		for(uint32 c : string)
			this->buffer.InsertTail(c);
	}

	inline void SkipWhiteSpaces(uint32 &codePoint)
	{
		while(StdXX::IsWhiteSpaceChar(codePoint))
			codePoint = this->ReadNextCodePointFromCurrentStream();
	}
};