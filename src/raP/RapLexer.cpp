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
//Class header
#include "RapLexer.hpp"
//Namespaces
using namespace StdXX;

//Public methods
String RapLexer::GetCurrentTokenValue()
{
	return this->tokenValue;
}

RapToken RapLexer::GetNextToken()
{
	this->SkipWhiteSpaces();

	uint32 codePoint = this->PeekAtNextChar();

	if(this->IsDigitChar(codePoint) || (codePoint == u8'-') || (codePoint == u8'.'))
		return this->ParseNumber();

	if(codePoint == u8'"')
		return this->ParseStringLiteral();

	if(this->IsDelimiterSymbol(codePoint))
		return this->ReadNextSymbol();

	this->tokenValue = {};
	return this->ParseFreeText();
}

//Private methods
bool RapLexer::IsDelimiterSymbol(uint32 codePoint) const
{
	switch(codePoint)
	{
		case 0:
		case u8'[':
		case u8'=':
		case u8'{':
		case u8'}':
		case u8':':
		case u8',':
		case u8';':
		case u8'"':
			return true;
	}
	return IsWhiteSpaceChar(codePoint);
}

RapToken RapLexer::ParseNumber()
{
	this->tokenValue = {};

	//sign
	if(this->PeekAtNextChar() == u8'-')
	{
		this->tokenValue += this->ConsumeNextChar();
		this->SkipWhiteSpaces();
	}

	//integer part
	while(this->IsDigitChar(this->PeekAtNextChar()))
		this->tokenValue += this->ConsumeNextChar();

	bool isFloat = false;

	//fractional part
	if(this->PeekAtNextChar() == u8'.')
	{
		isFloat = true;

		this->tokenValue += this->ConsumeNextChar();

		uint32 codePoint = this->ConsumeNextChar();
		if(this->IsDigitChar(codePoint))
		{
			this->tokenValue += codePoint;
			while(this->IsDigitChar(this->PeekAtNextChar()))
				this->tokenValue += this->ConsumeNextChar();
		}
		else
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
	}

	//exponent part
	if((this->PeekAtNextChar() == u8'e') || (this->PeekAtNextChar() == u8'E'))
	{
		isFloat = true;

		this->ConsumeNextChar();
		this->tokenValue += u8'e';

		//sign
		if(this->PeekAtNextChar() == u8'+')
			this->ConsumeNextChar();
		else if(this->PeekAtNextChar() == u8'-')
			this->tokenValue += this->ConsumeNextChar();

		//exponent
		uint32 codePoint = this->ConsumeNextChar();
		if(this->IsDigitChar(codePoint))
		{
			this->tokenValue += codePoint;
			while(this->IsDigitChar(this->PeekAtNextChar()))
				this->tokenValue += this->ConsumeNextChar();
		}
	}

	if(!this->IsDelimiterSymbol(this->PeekAtNextChar()))
		return this->ParseFreeText();

	if(isFloat)
		return TOKEN_FLOAT_LITERAL;
	return TOKEN_INT_LITERAL;
}

RapToken RapLexer::ParseFreeText()
{
	while(!this->IsDelimiterSymbol(this->PeekAtNextChar()))
		this->tokenValue += this->ConsumeNextChar();
	return TOKEN_TEXT;
}

RapToken RapLexer::ParseStringLiteral()
{
	this->ConsumeNextChar();

	this->tokenValue = {};
	while(true)
	{
		uint32 c = this->ConsumeNextChar();
		if(c == u8'"')
		{
			if(this->PeekAtNextChar() == u8'"')
				this->ConsumeNextChar();
			else
				break;
		}
		this->tokenValue += c;
	}
	return TOKEN_STRING_LITERAL;
}

RapToken RapLexer::ReadNextSymbol()
{
	switch(this->ConsumeNextChar())
	{
		case 0:
			return TOKEN_END;
		case u8'\n':
			return TOKEN_LINEBREAK;
		case u8'[':
			if(this->ConsumeNextChar() != u8']')
			NOT_IMPLEMENTED_ERROR; //TODO: implement me
			return TOKEN_ARRAY_MARKER;
		case u8'=':
			return TOKEN_ASSIGNMENT;
		case u8'{':
			return TOKEN_BRACEOPEN;
		case u8'}':
			return TOKEN_BRACECLOSE;
		case u8':':
			return TOKEN_COLON;
		case u8',':
			return TOKEN_COMMA;
		case u8';':
			return TOKEN_SEMICOLON;
	}
}