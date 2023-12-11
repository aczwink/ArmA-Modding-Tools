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
//Class header
#include "RapPreprocessor.hpp"

//Public methods
const libBISMod::RapParseContext &RapPreprocessor::QueryCurrentContext() const
{
	auto it = this->sourceStack.end();
	--it;
	while(true)
	{
		if(it.operator*().context.HasValue())
			return *it.operator*().context;
		if(it == this->sourceStack.begin())
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
		--it;
	}
}

uint32 RapPreprocessor::ReadNextCodePoint()
{
	if(!this->buffer.IsEmpty())
	{
		uint32 c = this->buffer.PopFront();
		return c;
	}

	uint32 codePoint = this->ReadNextCodePointFromCurrentStream();

	switch(codePoint)
	{
		case u8'\n':
			this->IncreaseLineCounter();
			return codePoint;
		case u8'#':
			return this->HandlePreprocessorDirective();
		case u8'/':
			return this->HandleSlash();
		case u8'"':
			{
				String tmp;
				tmp += u8'"';
				while((codePoint = this->ReadNextCodePointFromCurrentStream()) != u8'"')
					tmp += codePoint;
				tmp += u8'"';
				this->PutIntoBuffer(tmp);
				uint32 c = this->buffer.PopFront();
				return c;
			}
	}

	if(this->IsDelimiterSymbol(codePoint))
		return codePoint;

	String identifier;
	identifier += codePoint;

	while(true)
	{
		codePoint = this->ReadNextCodePointFromCurrentStream();
		if(!this->IsDelimiterSymbol(codePoint))
			identifier += codePoint;
		else
			break;
	}

	if(this->macros.Contains(identifier))
	{
		ASSERT_EQUALS(u8'(', codePoint);

		String currentArg;
		DynamicArray<String> args;
		while((codePoint = this->ReadNextCodePointFromCurrentStream()) != u8')')
		{
			if(codePoint == u8',')
			{
				args.Push(currentArg);
				currentArg = {};
				codePoint = this->ReadNextCodePointFromCurrentStream();
			}
			currentArg += codePoint;
		}
		args.Push(currentArg);

		const auto& t = this->macros.Get(identifier);

		this->PushDirectSource(t.Get<1>() + u8"/**/"); //unfortunately the comment is needed for macro expansion to work correctly :S otherwise the macroVarAssignments could be popped before the macro is fully expanded
		auto& sourceInfo = this->sourceStack.Last();

		for(uint32 i = 0; i < t.Get<0>().GetNumberOfElements(); i++)
		{
			//stdErr << u8"assigning " << args[i] << " to " << t.Get<0>()[i] << endl;
			sourceInfo.macroVarAssignments[t.Get<0>()[i]] = args[i];
		}

		return this->ReadNextCodePoint();
	}
	else
	{
		String expanded;
		if(this->TryResolveVariable(identifier, expanded))
		{
			this->PushCodePointSource(codePoint);

			this->PushDirectSource(expanded);
			return this->ReadNextCodePoint();
		}
	}

	this->PutIntoBuffer(identifier);
	this->PushCodePointSource(codePoint);
	uint32 c = this->buffer.PopFront();
	return c;
}

//Private methods
String RapPreprocessor::Expand(const String& value) const
{
	String result = value;
	if(this->TryResolveVariable(value, result))
		return result;
	return value;
}

uint32 RapPreprocessor::HandleDefine()
{
	uint32 codePoint = this->ReadNextCodePointFromCurrentStream();

	this->SkipWhiteSpaces(codePoint);

	String defName;
	defName += codePoint;
	while(true)
	{
		codePoint = this->ReadNextCodePointFromCurrentStream();
		if(IsWhiteSpaceChar(codePoint) || (codePoint == u8'('))
			break;
		defName += codePoint;
	}

	while(IsWhiteSpaceChar(codePoint) and !(codePoint == u8'\n'))
		codePoint = this->ReadNextCodePointFromCurrentStream();

	if(codePoint == u8'\n')
	{
		this->definitions.Insert(defName, {});
	}
	else if(codePoint == u8'(')
	{
		codePoint = this->ReadNextCodePointFromCurrentStream();
		this->SkipWhiteSpaces(codePoint);

		DynamicArray<String> varNames;
		String currentVarName;
		while(codePoint != u8')')
		{
			currentVarName += codePoint;
			codePoint = this->ReadNextCodePointFromCurrentStream();

			if(codePoint == u8',')
			{
				varNames.Push(currentVarName);
				currentVarName = {};
				codePoint = this->ReadNextCodePointFromCurrentStream();
			}
		}
		varNames.Push(currentVarName);

		codePoint = this->ReadNextCodePointFromCurrentStream();
		this->SkipWhiteSpaces(codePoint);

		this->macros.Insert(defName, {Move(varNames), this->ReadDefinitionValue(codePoint)});
	}
	else
		this->definitions.Insert(defName, this->ReadDefinitionValue(codePoint));

	return codePoint;
}

uint32 RapPreprocessor::HandlePreprocessorDirective()
{
	uint32 codePoint;
	String command = this->ReadPreprocessorDirective(codePoint);

	if(command == u8"define")
		return this->HandleDefine();
	else if(command == u8"else")
	{
		ASSERT_EQUALS(true, this->branches.Last()); //branch must have been taken
		return this->SkipBranch();
	}
	else if(command == u8"endif")
	{
		ASSERT_EQUALS(true, this->branches.Last()); //branch must have been taken
		this->branches.Pop();
		return codePoint;
	}
	else if(command == u8"ifdef")
	{
		String variable;
		while(!(codePoint == u8'\r' || codePoint == u8'\n'))
		{
			variable += codePoint;
			codePoint = this->ReadNextCodePointFromCurrentStream();
		}

		if(this->definitions.Contains(variable.Trim()))
		{
			this->branches.Push(true);
			return codePoint;
		}
		else
		{
			this->branches.Push(false);
			codePoint = this->SkipBranch();
			this->branches.Pop();
			return codePoint;
		}
	}
	else if(command == u8"include")
	{
		this->SkipWhiteSpaces(codePoint);
		ASSERT_EQUALS(u8'"', codePoint);

		String filePath;
		while(true)
		{
			codePoint = this->ReadNextCodePointFromCurrentStream();
			if(codePoint == u8'"')
			break;
			filePath += codePoint;
		}

		this->PushSource(filePath);
		return this->ReadNextCodePoint();
	}
	else if(command == u8"undef")
	{
		this->SkipWhiteSpaces(codePoint);

		String variable;
		while(!(codePoint == u8'\r' || codePoint == u8'\n'))
		{
			variable += codePoint;
			codePoint = this->ReadNextCodePointFromCurrentStream();
		}

		this->definitions.Remove(variable);
		this->macros.Remove(variable);
		return codePoint;
	}
	else if(codePoint == u8'#')
		return this->ReadNextCodePoint(); //concatenate two symbols
	else
	{
		String expanded;
		if(this->TryResolveVariable(command, expanded))
		{
			this->PushCodePointSource(codePoint);

			expanded = this->Expand(expanded);
			this->PushDirectSource(u8"\"" + expanded + u8"\"");
			return this->ReadNextCodePoint();
		}

		this->PushDirectSource(u8"\"" + command + u8"\"");
		return this->ReadNextCodePoint();
	}
}

uint32 RapPreprocessor::HandleSlash()
{
	uint32 codePoint;
	switch(codePoint = this->ReadNextCodePointFromCurrentStream())
	{
		case u8'/':
			while(true)
			{
				codePoint = this->ReadNextCodePointFromCurrentStream();
				if(codePoint == u8'\n')
				{
					this->IncreaseLineCounter();
					break;
				}
				if(codePoint == 0)
					break;
			}
			return codePoint;
		case u8'*':
			{
				while(true)
				{
					if (this->ReadNextCodePointFromCurrentStream() == u8'*')
					{
						if (this->ReadNextCodePointFromCurrentStream() == u8'/')
							return this->ReadNextCodePoint();
					}
				}
			}
		default:
			this->buffer.InsertTail(codePoint);
			return u8'/';
	}
}


bool RapPreprocessor::IsDelimiterSymbol(uint32 codePoint) const
{
	switch(codePoint)
	{
		case 0:
		case u8'=':
		case u8'.':
		case u8',':
		case u8':':
		case u8';':
		case u8'*':
		case u8'/':
		case u8'(':
		case u8'{':
		case u8'#':
		case u8'-':
			return true;
	}
	return IsWhiteSpaceChar(codePoint);
}

String RapPreprocessor::ReadDefinitionValue(uint32& codePoint)
{
	String value;
	while(!(codePoint == u8'\r' || codePoint == u8'\n'))
	{
		if(codePoint == u8'\\')
		{
			codePoint = this->ReadNextCodePointFromCurrentStream();
			if(codePoint == u8'\r')
				codePoint = this->ReadNextCodePointFromCurrentStream();

			if(codePoint == u8'\n')
				codePoint = this->ReadNextCodePointFromCurrentStream();
			else
				value += u8"\\";
		}
		else if(codePoint == u8'/')
		{
			codePoint = this->ReadNextCodePointFromCurrentStream();
			if(codePoint == u8'/')
			{
				while(codePoint != u8'\n')
					codePoint = this->ReadNextCodePointFromCurrentStream();
				continue;
			}
			else
				value += u8'/';
		}

		value += codePoint;
		codePoint = this->ReadNextCodePointFromCurrentStream();
	}

	return value.Trim(); //trailing whitespaces are not part of the define. That's important when using the # operator
}

uint32 RapPreprocessor::ReadNextCodePointFromCurrentStream()
{
	while(!this->sourceStack.IsEmpty() && this->sourceStack.Last().sourceStream->IsAtEnd())
		this->sourceStack.PopTail();

	if(this->sourceStack.IsEmpty())
		return 0;

	auto& source = this->sourceStack.Last();
	auto& stream = *source.sourceStream;

	//raP configs are usually written in latin1
	TextReader textReader(stream, source.textCodecType);
	uint32 c = textReader.ReadCodePoint();

	return c;
}

String RapPreprocessor::ReadPreprocessorDirective(uint32& codePoint)
{
	String command;

	while(true)
	{
		codePoint = this->ReadNextCodePointFromCurrentStream();

		if(this->IsDelimiterSymbol(codePoint))
			break;
		command += codePoint;
	}

	return command;
}

uint32 RapPreprocessor::SkipBranch()
{
	uint32 codePoint = 0;
	while(true)
	{
		while (codePoint != u8'#')
			codePoint = this->ReadNextCodePointFromCurrentStream();

		if(this->ReadPreprocessorDirective(codePoint) == u8"endif")
			break;
	}
	return codePoint;
}

bool RapPreprocessor::TryResolveVariable(const String &identifier, String &expanded) const
{
	if(this->definitions.Contains(identifier))
	{
		expanded = this->definitions.Get(identifier);

		return true;
	}

	auto it = this->sourceStack.end();
	do
	{
		--it;

		auto it2 = it.operator*().macroVarAssignments.Find(identifier);
		if(it2 != it.operator*().macroVarAssignments.end())
		{
			expanded = it2.operator*().value;

			if(expanded == identifier)
			{
				//recursive assignment
				continue;
			}

			return true;
		}
	}
	while(it != this->sourceStack.begin());

	return false;
}