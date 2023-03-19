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
#include <StdXXCore.hpp>

//Enums
enum EToken
{
	TOKEN_ILLEGAL,
	TOKEN_END,
	TOKEN_INVALID,
	TOKEN_TABSPACE,
	TOKEN_LINEFEED,
	TOKEN_CARRIAGERETURN,
	TOKEN_ASSIGNMENT,
	TOKEN_BACKSLASH,
	TOKEN_BRACEOPEN,
	TOKEN_BRACECLOSE,
	TOKEN_BRACKETOPEN,
	TOKEN_BRACKETCLOSE,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_DOUBLENUMBERSIGN,
	TOKEN_NUMBERSIGN,
	TOKEN_SEMICOLON,
	TOKEN_SQUAREDBRACKETOPEN,
	TOKEN_SQUAREDBRACKETCLOSE,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_IDENTIFIER,
	TOKEN_LITERAL,
	TOKEN_SINGLELINECOMMENT,
	TOKEN_MULTILINECOMMENTSTART,
	TOKEN_CLASS,
	TOKEN_DEFINE,
};

enum TokenState
{
	STATE_ILLEGAL,
	STATE_START,
	STATE_TABSPACE,
	STATE_LINEFEED,
	STATE_CARRIAGERETURN,
	STATE_UNCLOSEDLITERAL,
	STATE_CLOSEDLITERAL,
	STATE_NUMBERSIGN,
	STATE_DOUBLENUMBERSIGN,
	STATE_BRACKETOPEN,
	STATE_BRACKETCLOSE,
	STATE_PLUS,
	STATE_COMMA,
	STATE_MINUS,
	STATE_SLASH,
	STATE_SINGLELINECOMMENT,
	STATE_MULTILINECOMMENT,
	STATE_INTNUMBER,
	STATE_UNFINISHEDFLOAT,
	STATE_FLOAT,
	STATE_FLOATEXP,
	STATE_COLON,
	STATE_SEMICOLON,
	STATE_ASSIGNMENT,
	STATE_SQUAREDBRACKETOPEN,
	STATE_BACKSLASH,
	STATE_SQUAREDBRACKETCLOSE,
	STATE_LETTER,
	STATE_BRACEOPEN,
	STATE_BRACECLOSE,
	STATE_END,
	STATE_INVALID,
	STATE_FINISHED
};

enum EvalChar
{
	CHAR_ILLEGAL,
	CHAR_INVALID,
	CHAR_NULLBYTE,
	CHAR_TABSPACE,
	CHAR_LINEFEED,
	CHAR_CARRIAGERETURN,
	CHAR_INVERTEDCOMMA,
	CHAR_NUMBERSIGN,
	CHAR_BRACKETOPEN,
	CHAR_BRACKETCLOSE,
	CHAR_MULTIPLY,
	CHAR_PLUS,
	CHAR_COMMA,
	CHAR_MINUS,
	CHAR_DOT,
	CHAR_SLASH,
	CHAR_NUMBER,
	CHAR_COLON,
	CHAR_SEMICOLON,
	CHAR_ASSIGNMENT,
	CHAR_SQUAREDBRACKETOPEN,
	CHAR_BACKSLASH,
	CHAR_SQUAREDBRACKETCLOSE,
	CHAR_UNDERLINE,
	CHAR_SMALLE,
	CHAR_LETTER,
	CHAR_BRACEOPEN,
	CHAR_BRACECLOSE
};

class CRapLexer
{
public:
	//Constructor
	CRapLexer();

	//Methods
	StdXX::String GetCurrentLine();
	uint32 GetCurrentLineNumber();
	StdXX::String GetCurrentTokenValue();
	EToken GetNextToken(bool ignore = true);
	void Init(const char *pSource);

private:
	//State
	char *pSource;
	char *pCurrentLine;
	uint32 currentLineNumber;
	StdXX::String tokenValue;

	//Methods
	EvalChar EvaluateChar(char c);
	EToken GetTokenType(TokenState state);
	TokenState NextState(TokenState state, EvalChar c);
};