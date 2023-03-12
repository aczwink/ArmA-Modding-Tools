//Class Header
#include "__CSQFLexer.h"
//Namespaces
using namespace SJCLib;
using namespace BISLibrary;

//Constructor
CSQFLexer::CSQFLexer()
{
	this->pSource = NULL;
	this->pSourceStart = NULL;
}

//Destructor
CSQFLexer::~CSQFLexer()
{
}

//Private Functions
CharClass CSQFLexer::EvaluateChar(char c)
{
	switch(c)
	{
	case 0:
		return CC_NULLBYTE;
	case 9: //TAB
	case 32: //Space
		return CC_TABSPACE;
	case 10: //\n
		return CC_LINEFEED;
	case 13: //\r
		return CC_CARRIAGERETURN;
	case 33: //!
		return CC_NEGATION;
	case 34: //"
		return CC_INVERTEDCOMMA;
	case 38: //&
		return CC_AND;
	case 40: //(
		return CC_BRACKETOPEN;
	case 41: //)
		return CC_BRACKETCLOSE;
	case 42: //*
		return CC_MULTIPLY;
	case 43: //+
		return CC_PLUS;
	case 44: //,
		return CC_COMMA;
	case 46: //.
		return CC_DOT;
	case 47: ///
		return CC_SLASH;
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
		return CC_NUMBER;
	case 59: //;
		return CC_SEMICOLON;
	case 60: //<
		return CC_SMALLERTHAN;
	case 61: //=
		return CC_ASSIGNMENT;
	case 62: //>
		return CC_GREATERTHAN;
	case 91: //[
		return CC_SQUAREDBRACKETOPEN;
	case 92: /* \ */
		return CC_BACKSLASH;
	case 93: //]
		return CC_SQUAREDBRACKETCLOSE;
	case 95: //_
		return CC_UNDERSCORE;
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
	case 101: //e
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
		return CC_LETTER;
	case 123: //{
		return CC_BRACEOPEN;
	case 125: //}
		return CC_BRACECLOSE;
	case 37: //%
	case 63: //?
		return CC_INVALID;
	/*
	case 45: //-
		return CC_MINUS;
	case 58: //:
		return CC_COLON;
	/*case 94: //^
		return CC_INVALID;*/
	/*
	case 64: //@
		return CHAR_AT;
	case 124: //|
		return CHAR_OR;
	case 126: //~
		return CHAR_TILDE;*/
	/*
	case 36: //$
	case 39: //'
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
		case 96: //`
		case 127: //DEL
			return CHAR_ILLEGAL;
	case 35: //#
		*/
	}
	return CC_ILLEGAL;
}

SQFToken CSQFLexer::GetTokenType(SQFTokenState state)
{
	switch(state)
	{
	case SQF_STATE_INVALID:
		return SQF_TOKEN_INVALID;
	case SQF_STATE_END:
		return SQF_TOKEN_END;
	case SQF_STATE_TABSPACE:
		return SQF_TOKEN_TABSPACE;
	case SQF_STATE_CARRIAGERETURN:
		return SQF_TOKEN_CARRIAGERETURN;
	case SQF_STATE_LINEFEED:
		return SQF_TOKEN_LINEFEED;
	case SQF_STATE_NEGATION:
		return SQF_TOKEN_NEGATION;
	case SQF_STATE_LITERAL:
		return SQF_TOKEN_LITERAL;
	case SQF_STATE_LOGICALAND:
		return SQF_TOKEN_LOGICALAND;
	case SQF_STATE_BRACKETOPEN:
		return SQF_TOKEN_BRACKETOPEN;
	case SQF_STATE_BRACKETCLOSE:
		return SQF_TOKEN_BRACKETCLOSE;
	case SQF_STATE_MULTIPLY:
		return SQF_TOKEN_MULTIPLY;
	case SQF_STATE_PLUS:
		return SQF_TOKEN_PLUS;
	case SQF_STATE_SINGLELINECOMMENT:
		return SQF_TOKEN_SINGLELINECOMMENT;
	case SQF_STATE_MULTILINECOMMENT:
		return SQF_TOKEN_MULTILINECOMMENT;
	case SQF_STATE_COMMA:
		return SQF_TOKEN_COMMA;
	case SQF_STATE_NUMBER:
		return SQF_TOKEN_NUMBER;
	case SQF_STATE_FLOAT:
		return SQF_TOKEN_FLOAT;
	case SQF_STATE_SEMICOLON:
		return SQF_TOKEN_SEMICOLON;
	case SQF_STATE_ASSIGNMENT:
		return SQF_TOKEN_ASSIGNMENT;
	case SQF_STATE_EQUALS:
		return SQF_TOKEN_EQUALS;
	case SQF_STATE_NOTEQUALS:
		return SQF_TOKEN_NOTEQUALS;
	case SQF_STATE_LESSTHAN:
		return SQF_TOKEN_LESSTHAN;
	case SQF_STATE_GREATERTHAN:
		return SQF_TOKEN_GREATERTHAN;
	case SQF_STATE_SQUAREDBRACKETOPEN:
		return SQF_TOKEN_SQUAREDBRACKETOPEN;
	case SQF_STATE_SQUAREDBRACKETCLOSE:
		return SQF_TOKEN_SQUAREDBRACKETCLOSE;
	case SQF_STATE_PRIVATEVARIABLE:
		return SQF_TOKEN_PRIVATEVARIABLE;
	case SQF_STATE_LETTER:
		return SQF_TOKEN_IDENTIFIER;
	case SQF_STATE_BRACEOPEN:
		return SQF_TOKEN_BRACEOPEN;
	case SQF_STATE_BRACECLOSE:
		return SQF_TOKEN_BRACECLOSE;
	}
	return SQF_TOKEN_ILLEGAL;
}

SQFTokenState CSQFLexer::NextState(SQFTokenState state, CharClass c)
{
	switch(state)
	{
	case SQF_STATE_START:
		switch(c)
		{
		case CC_NULLBYTE:
			return SQF_STATE_END;
		case CC_DOT:
		case CC_INVALID:
			return SQF_STATE_INVALID;
		case CC_TABSPACE:
			return SQF_STATE_TABSPACE;
		case CC_LINEFEED:
			return SQF_STATE_LINEFEED;
		case CC_CARRIAGERETURN:
			return SQF_STATE_CARRIAGERETURN;
		case CC_NEGATION:
			return SQF_STATE_NEGATION;
		case CC_INVERTEDCOMMA:
			return SQF_STATE_UNCLOSEDLITERAL;
		case CC_AND:
			return SQF_STATE_AND;
		case CC_BRACKETOPEN:
			return SQF_STATE_BRACKETOPEN;
		case CC_BRACKETCLOSE:
			return SQF_STATE_BRACKETCLOSE;
		case CC_MULTIPLY:
			return SQF_STATE_MULTIPLY;
		case CC_PLUS:
			return SQF_STATE_PLUS;
		case CC_COMMA:
			return SQF_STATE_COMMA;
		case CC_SLASH:
			return SQF_STATE_SLASH;
		case CC_NUMBER:
			return SQF_STATE_NUMBER;
		case CC_SEMICOLON:
			return SQF_STATE_SEMICOLON;
		case CC_SMALLERTHAN:
			return SQF_STATE_LESSTHAN;
		case CC_ASSIGNMENT:
			return SQF_STATE_ASSIGNMENT;
		case CC_GREATERTHAN:
			return SQF_STATE_GREATERTHAN;
		case CC_SQUAREDBRACKETOPEN:
			return SQF_STATE_SQUAREDBRACKETOPEN;
		case CC_BACKSLASH:
			return SQF_STATE_INVALID;
		case CC_SQUAREDBRACKETCLOSE:
			return SQF_STATE_SQUAREDBRACKETCLOSE;
		case CC_UNDERSCORE:
			return SQF_STATE_UNDERSCORE;
		case CC_LETTER:
			return SQF_STATE_LETTER;
		case CC_BRACEOPEN:
			return SQF_STATE_BRACEOPEN;
		case CC_BRACECLOSE:
			return SQF_STATE_BRACECLOSE;
		}
		break;
	case SQF_STATE_TABSPACE:
		switch(c)
		{
		case CC_TABSPACE:
			return SQF_STATE_TABSPACE;
		case CC_NULLBYTE:
		case CC_LINEFEED:
		case CC_CARRIAGERETURN:
		case CC_NEGATION:
		case CC_INVERTEDCOMMA:
		case CC_BRACKETOPEN:
		case CC_BRACKETCLOSE:
		case CC_PLUS:
		case CC_SLASH:
		case CC_NUMBER:
		case CC_ASSIGNMENT:
		case CC_GREATERTHAN:
		case CC_SQUAREDBRACKETOPEN:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
		case CC_BRACECLOSE:
		case CC_INVALID:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_LINEFEED:
		switch(c)
		{
		case CC_LINEFEED:
			return SQF_STATE_LINEFEED;
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_CARRIAGERETURN:
		case CC_SLASH:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_CARRIAGERETURN:
		switch(c)
		{
		case CC_LINEFEED:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_NEGATION:
		switch(c)
		{
		case CC_ASSIGNMENT:
			return SQF_STATE_NOTEQUALS;
		case CC_NULLBYTE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_UNCLOSEDLITERAL:
		switch(c)
		{
		case CC_NULLBYTE:
			return SQF_STATE_INVALID;
		case CC_INVERTEDCOMMA:
			return SQF_STATE_LITERAL;
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_CARRIAGERETURN:
		case CC_NEGATION:
		case CC_AND:
		case CC_BRACKETOPEN:
		case CC_BRACKETCLOSE:
		case CC_MULTIPLY:
		case CC_PLUS:
		case CC_COMMA:
		case CC_DOT:
		case CC_SLASH:
		case CC_NUMBER:
		case CC_SEMICOLON:
		case CC_SMALLERTHAN:
		case CC_ASSIGNMENT:
		case CC_GREATERTHAN:
		case CC_SQUAREDBRACKETOPEN:
		case CC_BACKSLASH:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
		case CC_BRACECLOSE:
		case CC_INVALID:
			return SQF_STATE_UNCLOSEDLITERAL;
		}
		break;
	case SQF_STATE_LITERAL:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_BRACKETCLOSE:
		case CC_COMMA:
		case CC_SEMICOLON:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_LETTER:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_AND:
		switch(c)
		{
		case CC_AND:
			return SQF_STATE_LOGICALAND;
		}
		break;
	case SQF_STATE_LOGICALAND:
		switch(c)
		{
		case CC_BRACKETOPEN:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_BRACKETOPEN:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_BRACKETOPEN:
		case CC_NUMBER:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_BRACKETCLOSE:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_AND:
		case CC_BRACKETCLOSE:
		case CC_PLUS:
		case CC_SEMICOLON:
		case CC_SMALLERTHAN:
		case CC_GREATERTHAN:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_LETTER:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_MULTIPLY:
		switch(c)
		{
		case CC_NUMBER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_PLUS:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_LETTER:
		case CC_BRACEOPEN:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_COMMA:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_SLASH:
		switch(c)
		{
		case CC_MULTIPLY:
			return SQF_STATE_MULTILINECOMMENT;
		case CC_SLASH:
			return SQF_STATE_SINGLELINECOMMENT;
		case CC_NULLBYTE:
		case CC_LINEFEED:
			return SQF_STATE_INVALID;
		}
		break;
	case SQF_STATE_SINGLELINECOMMENT:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_UNDERSCORE:
		case CC_LETTER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_MULTILINECOMMENT:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_LINEFEED:
		case CC_MULTIPLY:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_NUMBER:
		switch(c)
		{
		case CC_DOT:
			return SQF_STATE_UNFINISHEDFLOAT;
		case CC_NUMBER:
			return SQF_STATE_NUMBER;
		case CC_LETTER:
			return SQF_STATE_LETTER;
		case CC_NULLBYTE:
		case CC_LINEFEED:
		case CC_BRACKETCLOSE:
		case CC_MULTIPLY:
		case CC_COMMA:
		case CC_SEMICOLON:
		case CC_SQUAREDBRACKETCLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_UNFINISHEDFLOAT:
		switch(c)
		{
		case CC_NUMBER:
			return SQF_STATE_FLOAT;
		}
		break;
	case SQF_STATE_FLOAT:
		switch(c)
		{
		case CC_NUMBER:
			return SQF_STATE_FLOAT;
		case CC_SEMICOLON:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_SEMICOLON:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_LETTER:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_ASSIGNMENT:
		switch(c)
		{
		case CC_ASSIGNMENT:
			return SQF_STATE_EQUALS;
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_NUMBER:
		case CC_UNDERSCORE:
		case CC_LETTER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_EQUALS:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_NUMBER:
		case CC_LETTER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_NOTEQUALS:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LETTER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_LESSTHAN:
		switch(c)
		{
		case CC_NUMBER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_GREATERTHAN:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_NUMBER:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_SQUAREDBRACKETOPEN:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_UNDERSCORE:
		case CC_BRACEOPEN:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_SQUAREDBRACKETCLOSE:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_NEGATION:
		case CC_BRACKETCLOSE:
		case CC_SEMICOLON:
		case CC_ASSIGNMENT:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_INVALID:
		switch(c)
		{
		case CC_NUMBER:
		case CC_BACKSLASH:
		case CC_UNDERSCORE:
		case CC_LETTER:
			return SQF_STATE_INVALID;
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_BRACKETOPEN:
		case CC_BRACEOPEN:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_UNDERSCORE:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_LINEFEED:
			return SQF_STATE_INVALID;
		case CC_LETTER:
			return SQF_STATE_PRIVATEVARIABLE;
		}
		break;
	case SQF_STATE_PRIVATEVARIABLE:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_BRACKETCLOSE:
		case CC_COMMA:
		case CC_SEMICOLON:
		case CC_ASSIGNMENT:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		case CC_LETTER:
			return SQF_STATE_PRIVATEVARIABLE;
		}
		break;
	case SQF_STATE_LETTER:
		switch(c)
		{
		case CC_NUMBER:
		case CC_UNDERSCORE:
		case CC_LETTER:
			return SQF_STATE_LETTER;
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_INVERTEDCOMMA:
		case CC_BRACKETOPEN:
		case CC_BRACKETCLOSE:
		case CC_PLUS:
		case CC_DOT:
		case CC_SLASH:
		case CC_SEMICOLON:
		case CC_ASSIGNMENT:
		case CC_GREATERTHAN:
		case CC_SQUAREDBRACKETOPEN:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_BRACEOPEN:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_BRACEOPEN:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_NUMBER:
		case CC_SQUAREDBRACKETOPEN:
		case CC_BACKSLASH:
		case CC_UNDERSCORE:
		case CC_LETTER:
		case CC_BRACEOPEN:
		case CC_INVALID:
			return SQF_STATE_FINISHED;
		}
		break;
	case SQF_STATE_BRACECLOSE:
		switch(c)
		{
		case CC_NULLBYTE:
		case CC_TABSPACE:
		case CC_LINEFEED:
		case CC_BRACKETCLOSE:
		case CC_COMMA:
		case CC_SEMICOLON:
		case CC_SQUAREDBRACKETCLOSE:
		case CC_LETTER:
		case CC_BRACECLOSE:
			return SQF_STATE_FINISHED;
		}
		break;
	}
	//this should never happen
	return SQF_STATE_ILLEGAL;
}

//Public Functions
CString CSQFLexer::GetCurrentTokenValue()
{
	return this->tokenValue;
}

void CSQFLexer::Init(const char *pSource)
{
	this->pSource = (char *)pSource;
	this->pSourceStart = pSource;
}

SQFToken CSQFLexer::GetNextToken(bool ignore)
{
	char *pStart;
	SQFTokenState state;
	SQFTokenState acceptedState;
	char *pAcceptedPos;
	SQFToken t;
	CharClass c;

start:
	pStart = this->pSource;
	state = SQF_STATE_START;

	while(state != SQF_STATE_FINISHED)
	{
		if(this->GetTokenType(state))
		{
			acceptedState = state;
			pAcceptedPos = this->pSource;

			if(state == SQF_STATE_END) break;
			if(state == SQF_STATE_INVALID && c == CC_NULLBYTE)
			{
				pAcceptedPos--;
				break;
			}
		}
		c = this->EvaluateChar(*this->pSource);
		state = this->NextState(state, c);
		this->pSource++;
	}
	this->pSource = pAcceptedPos;
	state = acceptedState;

	this->tokenValue.Assign(pStart, (uint32)(this->pSource - pStart));

	t = this->GetTokenType(state);
	switch(t)
	{
	case SQF_TOKEN_END:
		this->pSource--; //again points to the 0 byte so that any following call (which should not happen) to this function again returns TOKEN_END
		return SQF_TOKEN_END;
	case SQF_TOKEN_TABSPACE:
	case SQF_TOKEN_LINEFEED:
	case SQF_TOKEN_CARRIAGERETURN:
		//ignore
		if(!ignore)
		{
			return t;
		}
		goto start;
	case SQF_TOKEN_SINGLELINECOMMENT:
		while(true)
		{
			if(*this->pSource == '\n')
			{
				this->pSource++;
				break;
			}
			else if(*this->pSource == '\0')
			{
				break;
			}
			this->pSource++;
		}
		//ignore
		if(!ignore)
		{
			this->tokenValue.Assign(pStart, (uint32)(this->pSource - pStart));
			return t;
		}
		goto start;
	case SQF_TOKEN_MULTILINECOMMENT:
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
			else if(*this->pSource == '\0')
			{
				break;
			}
			this->pSource++;
		}
		//ignore
		if(!ignore)
		{
			this->tokenValue.Assign(pStart, (uint32)(this->pSource - pStart));
			return t;
		}
		goto start;
	default:
		return t;
	}
	return SQF_TOKEN_ILLEGAL;
}

uint32 CSQFLexer::GetCurrentOffset()
{
	return uint32(this->pSource - this->pSourceStart);
}