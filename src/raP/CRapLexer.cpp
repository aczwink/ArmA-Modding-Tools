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
#include "CRapLexer.hpp"
//Namespaces
using namespace StdXX;

//Global Variables
static String g_keywords[] =
{
		//preprocessor
		"define",
		//other
		"class"
};

//Internal Functions
static EToken GetKeywordToken(uint16 index)
{
	switch(index)
	{
		case 0:
			return TOKEN_DEFINE;
		case 1:
			return TOKEN_CLASS;
	}
	return TOKEN_ILLEGAL;
}

//Constructor
CRapLexer::CRapLexer()
{
	this->currentLineNumber = 1;
	this->pSource = NULL;
}

//Public methods
String CRapLexer::GetCurrentLine()
{
	String buffer;
	char *ptr;

	ptr = this->pCurrentLine;

	while(*ptr != '\n' && *ptr != '\0')
	{
		buffer += *ptr++;
	}

	return buffer;
}

uint32 CRapLexer::GetCurrentLineNumber()
{
	return this->currentLineNumber;
}

String CRapLexer::GetCurrentTokenValue()
{
	return this->tokenValue;
}

EToken CRapLexer::GetNextToken(bool ignore)
{
	char *pStart;
	TokenState state;
	TokenState acceptedState;
	char *pAcceptedPos;
	EToken t;
	EvalChar c;

	start:
	pStart = this->pSource;
	state = STATE_START;

	while(state != STATE_FINISHED)
	{
		if(this->GetTokenType(state))
		{
			acceptedState = state;
			pAcceptedPos = this->pSource;

			if(state == STATE_END) break;
		}
		c = this->EvaluateChar(*this->pSource);
		state = this->NextState(state, c);
		this->pSource++;
	}
	this->pSource = pAcceptedPos;
	state = acceptedState;

	this->tokenValue = String::CopyUtf8Bytes((const uint8*)pStart, (uint32)(this->pSource - pStart));

	t = this->GetTokenType(state);
	switch(t)
	{
		case TOKEN_LINEFEED:
			this->currentLineNumber++;
			this->pCurrentLine = this->pSource;
		case TOKEN_TABSPACE:
		case TOKEN_CARRIAGERETURN:
			//ignore
			if(!ignore)
			{
				return t;
			}
			goto start;
		case TOKEN_MULTILINECOMMENTSTART:
			while(true)
			{
				if(*this->pSource == '*')
				{
					if(*(this->pSource+1) == '/')
					{
						this->pSource += 2;
						break;
					}
				}
				if(*this->pSource == '\n')
				{
					this->currentLineNumber++;
				}
				if(*this->pSource == 0)
				{
					break;
				}
				this->pSource++;
			}
			//ignore
			goto start;
		case TOKEN_SINGLELINECOMMENT:
			while(*this->pSource != '\n' && *this->pSource != 0) this->pSource++;
			if(*this->pSource != 0)
			{
				this->pSource++;
				this->currentLineNumber++;
			}
			//ignore
			goto start;
		case TOKEN_IDENTIFIER:
		{
			for(uint32 i = 0; i < sizeof(g_keywords)/sizeof(g_keywords[0]); i++)
			{
				if(this->tokenValue == g_keywords[i])
				{
					return GetKeywordToken(i);
				}
			}
			return TOKEN_IDENTIFIER;
		}
			break;
		case TOKEN_END:
			this->pSource--; //again points to the 0 byte so that any following call (which should not happen) to this function again returns TOKEN_END
		default:
			return t;
	}
	return TOKEN_ILLEGAL;
}

void CRapLexer::Init(const char *pSource)
{
	this->pSource = (char *)pSource;
	this->pCurrentLine = this->pSource;
}

//Private methods
EvalChar CRapLexer::EvaluateChar(char c)
{
	switch(c)
	{
		case 0:
			return CHAR_NULLBYTE;
		case 9: //TAB
		case 32: //Space
			return CHAR_TABSPACE;
		case 10: //\n
			return CHAR_LINEFEED;
		case 13: //\r
			return CHAR_CARRIAGERETURN;
		case 34: //"
			return CHAR_INVERTEDCOMMA;
		case 35: //#
			return CHAR_NUMBERSIGN;
		case 40: //(
			return CHAR_BRACKETOPEN;
		case 41: //)
			return CHAR_BRACKETCLOSE;
		case 42: //*
			return CHAR_MULTIPLY;
		case 43: //+
			return CHAR_PLUS;
		case 44: //,
			return CHAR_COMMA;
		case 45: //-
			return CHAR_MINUS;
		case 46: //.
			return CHAR_DOT;
		case 47: ///
			return CHAR_SLASH;
		case 48: //0
		case 49: //1
		case 50: //2
		case 51: //3
		case 52: //4
		case 53: //5
		case 54: //6
		case 55: //7
		case 56: //8
		case 57: //9
			return CHAR_NUMBER;
		case 58: //:
			return CHAR_COLON;
		case 59: //;
			return CHAR_SEMICOLON;
		case 61: //=
			return CHAR_ASSIGNMENT;
		case 91: //[
			return CHAR_SQUAREDBRACKETOPEN;
		case 92: /* \ */
			return CHAR_BACKSLASH;
		case 93: //]
			return CHAR_SQUAREDBRACKETCLOSE;
		case 95: //_
			return CHAR_UNDERLINE;
		case 101: //e
			return CHAR_SMALLE;
		case 65: //A
		case 66: //B
		case 67: //C
		case 68: //D
		case 69: //E
		case 70: //F
		case 71: //G
		case 72: //H
		case 73: //I
		case 74: //J
		case 75: //K
		case 76: //L
		case 77: //M
		case 78: //N
		case 79: //O
		case 80: //P
		case 81: //Q
		case 82: //R
		case 83: //S
		case 84: //T
		case 85: //U
		case 86: //V
		case 87: //W
		case 88: //X
		case 89: //Y
		case 90: //Z
		case 97: //a
		case 98: //b
		case 99: //c
		case 100: //d
		case 102: //f
		case 103: //g
		case 104: //h
		case 105: //i
		case 106: //j
		case 107: //k
		case 108: //l
		case 109: //m
		case 110: //n
		case 111: //o
		case 112: //p
		case 113: //q
		case 114: //r
		case 115: //s
		case 116: //t
		case 117: //u
		case 118: //v
		case 119: //w
		case 120: //x
		case 121: //y
		case 122: //z
			return CHAR_LETTER;
		case 123: //{
			return CHAR_BRACEOPEN;
		case 125: //}
			return CHAR_BRACECLOSE;
		case 33: //!
		case 36: //$
		case 39: //'
		case 63: //?
			return CHAR_INVALID;
			/*
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 11:
			case 12:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 64: //@
			case 94: //^
			case 96: //`
			case 126: //~
			case 127: //DEL
				return CHAR_ILLEGAL;
			case 37: //%
				return CHAR_MODULO;
			case 38: //&
				return CHAR_AND;
			case 60: //<
				return CHAR_SMALLERTHAN;
			case 62: //>
				return CHAR_GREATERTHAN;
			case 124: //|
				return CHAR_OR;
			*/
	}
	return CHAR_ILLEGAL;
}

EToken CRapLexer::GetTokenType(TokenState state)
{
	switch(state)
	{
		case STATE_TABSPACE:
			return TOKEN_TABSPACE;
		case STATE_LINEFEED:
			return TOKEN_LINEFEED;
		case STATE_CARRIAGERETURN:
			return TOKEN_CARRIAGERETURN;
		case STATE_CLOSEDLITERAL:
			return TOKEN_LITERAL;
		case STATE_NUMBERSIGN:
			return TOKEN_NUMBERSIGN;
		case STATE_DOUBLENUMBERSIGN:
			return TOKEN_DOUBLENUMBERSIGN;
		case STATE_BRACKETOPEN:
			return TOKEN_BRACKETOPEN;
		case STATE_BRACKETCLOSE:
			return TOKEN_BRACKETCLOSE;
		case STATE_COMMA:
			return TOKEN_COMMA;
		case STATE_SINGLELINECOMMENT:
			return TOKEN_SINGLELINECOMMENT;
		case STATE_MULTILINECOMMENT:
			return TOKEN_MULTILINECOMMENTSTART;
		case STATE_INTNUMBER:
			return TOKEN_INT;
		case STATE_FLOAT:
		case STATE_FLOATEXP:
			return TOKEN_FLOAT;
		case STATE_COLON:
			return TOKEN_COLON;
		case STATE_SEMICOLON:
			return TOKEN_SEMICOLON;
		case STATE_ASSIGNMENT:
			return TOKEN_ASSIGNMENT;
		case STATE_SQUAREDBRACKETOPEN:
			return TOKEN_SQUAREDBRACKETOPEN;
		case STATE_BACKSLASH:
			return TOKEN_BACKSLASH;
		case STATE_SQUAREDBRACKETCLOSE:
			return TOKEN_SQUAREDBRACKETCLOSE;
		case STATE_LETTER:
			return TOKEN_IDENTIFIER;
		case STATE_BRACEOPEN:
			return TOKEN_BRACEOPEN;
		case STATE_BRACECLOSE:
			return TOKEN_BRACECLOSE;
		case STATE_END:
			return TOKEN_END;
		case STATE_MINUS:
		case STATE_INVALID:
			return TOKEN_INVALID;
	}
	return TOKEN_ILLEGAL;
}

TokenState CRapLexer::NextState(TokenState state, EvalChar c)
{
	switch(state)
	{
		case STATE_START:
			switch(c)
			{
				case CHAR_INVALID:
				case CHAR_DOT:
					return STATE_INVALID;
				case CHAR_NULLBYTE:
					return STATE_END;
				case CHAR_TABSPACE:
					return STATE_TABSPACE;
				case CHAR_LINEFEED:
					return STATE_LINEFEED;
				case CHAR_CARRIAGERETURN:
					return STATE_CARRIAGERETURN;
				case CHAR_INVERTEDCOMMA:
					return STATE_UNCLOSEDLITERAL;
				case CHAR_NUMBERSIGN:
					return STATE_NUMBERSIGN;
				case CHAR_BRACKETOPEN:
					return STATE_BRACKETOPEN;
				case CHAR_BRACKETCLOSE:
					return STATE_BRACKETCLOSE;
				case CHAR_PLUS:
					return STATE_PLUS;
				case CHAR_COMMA:
					return STATE_COMMA;
				case CHAR_MINUS:
					return STATE_MINUS;
				case CHAR_SLASH:
					return STATE_SLASH;
				case CHAR_NUMBER:
					return STATE_INTNUMBER;
				case CHAR_COLON:
					return STATE_COLON;
				case CHAR_SEMICOLON:
					return STATE_SEMICOLON;
				case CHAR_ASSIGNMENT:
					return STATE_ASSIGNMENT;
				case CHAR_SQUAREDBRACKETOPEN:
					return STATE_SQUAREDBRACKETOPEN;
				case CHAR_BACKSLASH:
					return STATE_BACKSLASH;
				case CHAR_SQUAREDBRACKETCLOSE:
					return STATE_SQUAREDBRACKETCLOSE;
				case CHAR_UNDERLINE:
				case CHAR_SMALLE:
				case CHAR_LETTER:
					return STATE_LETTER;
				case CHAR_BRACEOPEN:
					return STATE_BRACEOPEN;
				case CHAR_BRACECLOSE:
					return STATE_BRACECLOSE;
			}
			break;
		case STATE_TABSPACE:
			switch(c)
			{
				case CHAR_TABSPACE:
					return STATE_TABSPACE;
				case CHAR_INVERTEDCOMMA:
				case CHAR_MINUS:
				case CHAR_NUMBER:
				case CHAR_COLON:
				case CHAR_ASSIGNMENT:
				case CHAR_UNDERLINE:
				case CHAR_SMALLE:
				case CHAR_LETTER:
				case CHAR_BRACEOPEN:
				case CHAR_BRACECLOSE:
					return STATE_FINISHED;
			}
			break;
		case STATE_UNCLOSEDLITERAL:
			switch(c)
			{
				case CHAR_INVERTEDCOMMA:
					return STATE_CLOSEDLITERAL;
				case CHAR_INVALID:
				case CHAR_TABSPACE:
				case CHAR_NUMBERSIGN:
				case CHAR_MULTIPLY:
				case CHAR_PLUS:
				case CHAR_COMMA:
				case CHAR_MINUS:
				case CHAR_DOT:
				case CHAR_SLASH:
				case CHAR_NUMBER:
				case CHAR_COLON:
				case CHAR_BACKSLASH:
				case CHAR_UNDERLINE:
				case CHAR_SMALLE:
				case CHAR_LETTER:
				case CHAR_BRACEOPEN:
				case CHAR_BRACECLOSE:
					return STATE_UNCLOSEDLITERAL;
			}
			break;
		case STATE_NUMBERSIGN:
			switch(c)
			{
				case CHAR_NUMBERSIGN:
					return STATE_DOUBLENUMBERSIGN;
				case CHAR_BRACKETOPEN:
				case CHAR_LETTER:
					return STATE_FINISHED;
			}
			break;
		case STATE_PLUS:
			switch(c)
			{
				case CHAR_NUMBER:
					return STATE_INTNUMBER;
			}
			break;
		case STATE_MINUS:
			switch(c)
			{
				case CHAR_TABSPACE:
					return STATE_MINUS;
				case CHAR_NUMBER:
					return STATE_INTNUMBER;
				case CHAR_LETTER:
					return STATE_FINISHED;
			}
			break;
		case STATE_SLASH:
			switch(c)
			{
				case CHAR_MULTIPLY:
					return STATE_MULTILINECOMMENT;
				case CHAR_SLASH:
					return STATE_SINGLELINECOMMENT;
			}
			break;
		case STATE_INTNUMBER:
			switch(c)
			{
				case CHAR_DOT:
					return STATE_UNFINISHEDFLOAT;
				case CHAR_NUMBER:
					return STATE_INTNUMBER;
				case CHAR_SMALLE:
					return STATE_FLOATEXP;
				case CHAR_LETTER:
					return STATE_LETTER;
				case CHAR_CARRIAGERETURN:
				case CHAR_COMMA:
				case CHAR_SLASH:
				case CHAR_SEMICOLON:
				case CHAR_BRACECLOSE:
					return STATE_FINISHED;
			}
			break;
		case STATE_UNFINISHEDFLOAT:
			if(c == CHAR_NUMBER)
			{
				return STATE_FLOAT;
			}
			break;
		case STATE_FLOAT:
			switch(c)
			{
				case CHAR_NUMBER:
					return STATE_FLOAT;
				case CHAR_SMALLE:
					return STATE_FLOATEXP;
				case CHAR_COMMA:
				case CHAR_SEMICOLON:
				case CHAR_BRACECLOSE:
					return STATE_FINISHED;
			}
			break;
		case STATE_FLOATEXP:
			switch(c)
			{
				case CHAR_PLUS:
				case CHAR_MINUS:
				case CHAR_NUMBER:
					return STATE_FLOATEXP;
				case CHAR_CARRIAGERETURN:
				case CHAR_COMMA:
				case CHAR_SEMICOLON:
					return STATE_FINISHED;
			}
			break;
		case STATE_LETTER:
			switch(c)
			{
				case CHAR_TABSPACE:
				case CHAR_CARRIAGERETURN:
				case CHAR_NUMBERSIGN:
				case CHAR_BRACKETOPEN:
				case CHAR_BRACKETCLOSE:
				case CHAR_COMMA:
				case CHAR_DOT:
				case CHAR_COLON:
				case CHAR_SEMICOLON:
				case CHAR_ASSIGNMENT:
				case CHAR_SQUAREDBRACKETOPEN:
				case CHAR_BRACECLOSE:
					return STATE_FINISHED;
				case CHAR_NUMBER:
				case CHAR_UNDERLINE:
				case CHAR_SMALLE:
				case CHAR_LETTER:
					return STATE_LETTER;
				case CHAR_INVALID:
				case CHAR_BACKSLASH:
					return STATE_INVALID;
			}
			break;
		case STATE_LINEFEED:
		case STATE_CARRIAGERETURN:
		case STATE_CLOSEDLITERAL:
		case STATE_DOUBLENUMBERSIGN:
		case STATE_BRACKETOPEN:
		case STATE_BRACKETCLOSE:
		case STATE_COMMA:
		case STATE_SINGLELINECOMMENT:
		case STATE_MULTILINECOMMENT:
		case STATE_COLON:
		case STATE_SEMICOLON:
		case STATE_ASSIGNMENT:
		case STATE_SQUAREDBRACKETOPEN:
		case STATE_BACKSLASH:
		case STATE_SQUAREDBRACKETCLOSE:
		case STATE_BRACEOPEN:
		case STATE_BRACECLOSE:
		case STATE_INVALID:
			return STATE_FINISHED;
	}
	//this should never happen
	return STATE_ILLEGAL;
}