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
#include <StdXXCore.hpp>
#include "RapPreprocessor.hpp"
#include "ContextAwareCharStreamReader.hpp"

//Enums
enum RapToken
{
	TOKEN_END,
	//syntax symbols
	TOKEN_ARRAY_MARKER,
	TOKEN_ASSIGNMENT,
	TOKEN_BRACEOPEN,
	TOKEN_BRACECLOSE,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_LINEBREAK,
	TOKEN_SEMICOLON,
	//tokens with value
	TOKEN_INT_LITERAL,
	TOKEN_FLOAT_LITERAL,
	TOKEN_STRING_LITERAL,
	TOKEN_TEXT,
};

class RapLexer
{
public:
	//Constructor
	inline RapLexer(ContextAwareCharStreamReader& charStreamReader): charStreamReader(charStreamReader)
	{
		this->nextChar = charStreamReader.ReadNextCodePoint();
	}

	//Properties
	inline libBISMod::RapParseContext CurrentContext() const
	{
		return this->charStreamReader.QueryCurrentContext();
	}

	//Methods
	StdXX::String GetCurrentTokenValue();
	RapToken GetNextToken();

private:
	//State
	uint32 nextChar;
	ContextAwareCharStreamReader& charStreamReader;
	StdXX::String tokenValue;

	//Methods
	bool IsDelimiterSymbol(uint32 codePoint) const;
	RapToken ParseNumber();
	RapToken ParseFreeText();
	RapToken ParseStringLiteral();
	RapToken ReadNextSymbol();

	//Inline
	inline uint32 ConsumeNextChar()
	{
		uint32 tmp = this->nextChar;
		this->nextChar = this->charStreamReader.ReadNextCodePoint();
		return tmp;
	}

	inline bool IsDigitChar(uint32 codePoint) const
	{
		return Math::IsValueInInterval(codePoint, (uint32)u8'0', (uint32)u8'9');
	}

	inline bool IsWhiteSpace(uint32 codePoint) const
	{
		return (codePoint != u8'\n') && StdXX::IsWhiteSpaceChar(codePoint);
	}

	inline uint32 PeekAtNextChar() const
	{
		return this->nextChar;
	}

	inline void SkipWhiteSpaces()
	{
		while(this->IsWhiteSpace(this->PeekAtNextChar()))
			this->ConsumeNextChar();
	}
};