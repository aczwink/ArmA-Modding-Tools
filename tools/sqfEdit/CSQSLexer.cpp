//Class Header
#include "__CSQSLexer.h"
//Namespaces
using namespace SJCLib;
using namespace BISLibrary;

//Constructor
CSQSLexer::CSQSLexer()
{
	this->pSource = NULL;
	this->pSourceStart = NULL;
}

//Destructor
CSQSLexer::~CSQSLexer()
{
}

//Private Functions
EvalChar CSQSLexer::EvaluateChar(char c)
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
	case 33: //!
		return CHAR_NEGATION;
	case 34: //"
		return CHAR_INVERTEDCOMMA;
	case 35: //#
		return CHAR_NUMBERSIGN;
	case 38: //&
		return CHAR_AND;
	case 40: //(
		return CHAR_BRACKETOPEN;
	case 41: //)
		return CHAR_BRACKETCLOSE;
	case 42: //*
		return SQS_CC_MULTIPLY;
	case 43: //+
		return CHAR_PLUS;
	case 44: //,
		return CHAR_COMMA;
	case 45: //-
		return CHAR_MINUS;
	case 46: //.
		return CHAR_DOT;
	case 47: ///
		return SQS_CC_SLASH;
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
	case 60: //<
		return CC_LESSTHAN;
	case 61: //=
		return CHAR_ASSIGNMENT;
	case 62: //>
		return CHAR_GREATERTHAN;
	case 63: //?
		return CHAR_QUESTIONMARK;
	case 64: //@
		return CHAR_AT;
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
		return CHAR_LETTER;
	case 91: //[
		return CHAR_SQUAREDBRACKETOPEN;
	case 93: //]
		return CHAR_SQUAREDBRACKETCLOSE;
	case 95: //_
		return CHAR_UNDERLINE;
	case 123: //{
		return CHAR_BRACEOPEN;
	case 124: //|
		return CHAR_OR;
	case 125: //}
		return CHAR_BRACECLOSE;
	case 126: //~
		return CHAR_TILDE;
	case 37: //%
	case 92: /* \ */
		return CHAR_INVALID;
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
		case 94: //^
		case 96: //`
		case 127: //DEL
			return CHAR_ILLEGAL;
		*/
	}
	return CHAR_ILLEGAL;
}

Token CSQSLexer::GetTokenType(TokenState state)
{
	switch(state)
	{
	case STATE_END:
		return TOKEN_END;
	case STATE_TABSPACE:
		return TOKEN_TABSPACE;
	case STATE_CARRIAGERETURN:
		return TOKEN_CARRIAGERETURN;
	case STATE_LINEFEED:
		return TOKEN_LINEFEED;
	case STATE_NEGATION:
		return TOKEN_NEGATION;
	case STATE_EQUALS:
		return TOKEN_EQUALS;
	case STATE_NOTEQUALS:
		return TOKEN_NOTEQUALS;
	case STATE_CLOSEDLITERAL:
	case STATE_CLOSEDBRACELITERAL:
		return TOKEN_LITERAL;
	case STATE_WAITFORTIME:
		return TOKEN_WAITFORTIME;
	case SQS_STATE_LOGICALAND:
		return SQS_TOKEN_LOGICALAND;
	case STATE_LABEL:
		return TOKEN_LABEL;
	case STATE_BRACKETOPEN:
		return TOKEN_BRACKETOPEN;
	case STATE_BRACKETCLOSE:
		return TOKEN_BRACKETCLOSE;
	case SQS_STATE_MULTIPLY:
		return SQS_TOKEN_MULTIPLY;
	case STATE_PLUS:
		return TOKEN_PLUS;
	case STATE_COMMA:
		return TOKEN_COMMA;
	case STATE_MINUS:
		return SQS_TOKEN_MINUS;
	case SQS_STATE_SLASH:
		return SQS_TOKEN_SLASH;
	case STATE_NUMBER:
		return TOKEN_INT;
	case STATE_FLOAT:
		return TOKEN_FLOAT;
	case STATE_COLON:
		return TOKEN_COLON;
	case STATE_SEMICOLON:
		return TOKEN_SEMICOLON;
	case SQS_STATE_LESSTHAN:
		return SQS_TOKEN_LESSTHAN;
	case SQS_STATE_LESSOREQUAL:
		return SQS_TOKEN_LESSOREQUAL;
	case STATE_ASSIGNMENT:
		return TOKEN_ASSIGNMENT;
	case STATE_GREATERTHAN:
		return TOKEN_GREATERTHAN;
	case SQS_STATE_GREATEROREQUAL:
		return SQS_TOKEN_GREATEROREQUAL;
	case STATE_QUESTIONMARK:
		return TOKEN_QUESTIONMARK;
	case STATE_AT:
		return TOKEN_WAITFORCONDITION;
	case STATE_LETTER:
		return TOKEN_IDENTIFIER;
	case STATE_SQUAREDBRACKETOPEN:
		return TOKEN_SQUAREDBRACKETOPEN;
	case STATE_SQUAREDBRACKETCLOSE:
		return TOKEN_SQUAREDBRACKETCLOSE;
	case STATE_BRACEOPEN:
		return TOKEN_BRACEOPEN;
	case STATE_BRACECLOSE:
		return TOKEN_BRACECLOSE;
	case STATE_LOGICALOR:
		return TOKEN_LOGICALOR;
	case STATE_TILDE:
		return TOKEN_DELAY;
	case STATE_PRIVATEVARIABLE:
		return TOKEN_PRIVATEVARIABLE;
	case STATE_INVALID:
		return TOKEN_INVALID;
	}
	return TOKEN_ILLEGAL;
}

TokenState CSQSLexer::NextState(TokenState state, EvalChar c)
{
	switch(state)
	{
	case STATE_START:
		switch(c)
		{
		case CHAR_NULLBYTE:
			return STATE_END;
		case CHAR_TABSPACE:
			return STATE_TABSPACE;
		case CHAR_CARRIAGERETURN:
			return STATE_CARRIAGERETURN;
		case CHAR_LINEFEED:
			return STATE_LINEFEED;
		case CHAR_NEGATION:
			return STATE_NEGATION;
		case CHAR_INVERTEDCOMMA:
			return STATE_UNCLOSEDLITERAL;
		case CHAR_NUMBERSIGN:
			return STATE_NUMBERSIGN;
		case CHAR_AND:
			return STATE_WAITFORTIME;
		case CHAR_BRACKETOPEN:
			return STATE_BRACKETOPEN;
		case CHAR_BRACKETCLOSE:
			return STATE_BRACKETCLOSE;
		case SQS_CC_MULTIPLY:
			return SQS_STATE_MULTIPLY;
		case CHAR_PLUS:
			return STATE_PLUS;
		case CHAR_COMMA:
			return STATE_COMMA;
		case CHAR_MINUS:
			return STATE_MINUS;
		case SQS_CC_SLASH:
			return SQS_STATE_SLASH;
		case CHAR_NUMBER:
			return STATE_NUMBER;
		case CHAR_COLON:
			return STATE_COLON;
		case CHAR_SEMICOLON:
			return STATE_SEMICOLON;
		case CC_LESSTHAN:
			return SQS_STATE_LESSTHAN;
		case CHAR_ASSIGNMENT:
			return STATE_ASSIGNMENT;
		case CHAR_GREATERTHAN:
			return STATE_GREATERTHAN;
		case CHAR_QUESTIONMARK:
			return STATE_QUESTIONMARK;
		case CHAR_AT:
			return STATE_AT;
		case CHAR_LETTER:
			return STATE_LETTER;
		case CHAR_SQUAREDBRACKETOPEN:
			return STATE_SQUAREDBRACKETOPEN;
		case CHAR_SQUAREDBRACKETCLOSE:
			return STATE_SQUAREDBRACKETCLOSE;
		case CHAR_UNDERLINE:
			return STATE_UNDERLINE;
		case CHAR_BRACEOPEN:
			return STATE_BRACEOPEN;
		case CHAR_OR:
			return STATE_OR;
		case CHAR_BRACECLOSE:
			return STATE_BRACECLOSE;
		case CHAR_TILDE:
			return STATE_TILDE;
		case CHAR_INVALID:
			return STATE_INVALID;
		}
		break;
	case STATE_TABSPACE:
		switch(c)
		{
		case CHAR_TABSPACE:
			return STATE_TABSPACE;
		case CHAR_NULLBYTE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NEGATION:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_BRACKETCLOSE:
		case CHAR_PLUS:
		case CHAR_MINUS:
		case SQS_CC_SLASH:
		case CHAR_NUMBER:
		case CHAR_COLON:
		case CHAR_SEMICOLON:
		case CC_LESSTHAN:
		case CHAR_ASSIGNMENT:
		case CHAR_GREATERTHAN:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
		case CHAR_OR:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_CARRIAGERETURN:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	case STATE_LINEFEED:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NUMBERSIGN:
		case CHAR_AND:
		case CHAR_NUMBER:
		case CHAR_SEMICOLON:
		case CHAR_QUESTIONMARK:
		case CHAR_AT:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
		case CHAR_TILDE:
			return STATE_FINISHED;
		}
		break;
	case STATE_NEGATION:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		case CHAR_ASSIGNMENT:
			return STATE_NOTEQUALS;
		}
		break;
	case STATE_EQUALS:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
			return STATE_FINISHED;
		}
		break;
	case STATE_NOTEQUALS:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_UNCLOSEDLITERAL:
		switch(c)
		{
		case CHAR_NULLBYTE:
			return STATE_INVALID;
		case CHAR_TABSPACE:
		case CHAR_NEGATION:
		case CHAR_NUMBERSIGN:
		case CHAR_BRACKETOPEN:
		case CHAR_BRACKETCLOSE:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case CHAR_DOT:
		case CHAR_NUMBER:
		case CHAR_COLON:
		case CHAR_ASSIGNMENT:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_UNDERLINE:
		case CHAR_BRACECLOSE:
		case CHAR_INVALID:
			return STATE_UNCLOSEDLITERAL;
		case CHAR_INVERTEDCOMMA:
			return STATE_CLOSEDLITERAL;
		}
		break;
	case STATE_CLOSEDLITERAL:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_BRACKETCLOSE:
		case CHAR_COMMA:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_UNCLOSEDBRACELITERAL:
		switch(c)
		{
		case CHAR_DOT:
		case CHAR_NUMBER:
		case CHAR_AT:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
		case CHAR_INVALID:
			return STATE_UNCLOSEDBRACELITERAL;
		case CHAR_BRACECLOSE:
			return STATE_CLOSEDBRACELITERAL;
		}
		break;
	case STATE_CLOSEDBRACELITERAL:
		switch(c)
		{
		case CHAR_COMMA:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_NUMBERSIGN:
		switch(c)
		{
		case CHAR_LETTER:
			return STATE_LABEL;
		}
		break;
	case STATE_WAITFORTIME:
		switch(c)
		{
		case CHAR_AND:
			return SQS_STATE_LOGICALAND;
		case CHAR_NUMBER:
			return STATE_FINISHED;
		}
		break;
	case SQS_STATE_LOGICALAND:
		switch(c)
		{
		case CHAR_NEGATION:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_LABEL:
		switch(c)
		{
		case CHAR_NUMBER:
		case CHAR_LETTER:
			return STATE_LABEL;
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
			return STATE_FINISHED;
		}
		break;
	case STATE_BRACKETOPEN:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_NEGATION:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
			return STATE_FINISHED;
		}
		break;
	case STATE_BRACKETCLOSE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_AND:
		case CHAR_BRACKETCLOSE:
		case SQS_CC_MULTIPLY:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case SQS_CC_SLASH:
		case CHAR_COLON:
		case CHAR_SEMICOLON:
		case CC_LESSTHAN:
		case CHAR_ASSIGNMENT:
		case CHAR_GREATERTHAN:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_OR:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case SQS_STATE_MULTIPLY:
		switch(c)
		{
		case CHAR_NUMBER:
		case CHAR_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_PLUS:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
			return STATE_FINISHED;
		}
		break;
	case STATE_COMMA:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_MINUS:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_MINUS:
		switch(c)
		{
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
			return STATE_FINISHED;
		case CHAR_NUMBER:
			return STATE_NUMBER;
		}
		break;
	case SQS_STATE_SLASH:
		switch(c)
		{
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
			return STATE_FINISHED;
		}
		break;
	case STATE_NUMBER:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_AND:
		case CHAR_BRACKETCLOSE:
		case SQS_CC_MULTIPLY:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case CHAR_MINUS:
		case CHAR_COLON:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_OR:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		case CHAR_NUMBER:
			return STATE_NUMBER;
		case CHAR_DOT:
			return STATE_UNFINISHEDFLOAT;
		}
		break;
	case STATE_UNFINISHEDFLOAT:
		switch(c)
		{
		case CHAR_NULLBYTE:
			return STATE_INVALID;
		case CHAR_NUMBER:
			return STATE_FLOAT;
		}
		break;
	case STATE_FLOAT:
		switch(c)
		{
		case CHAR_NUMBER:
			return STATE_FLOAT;
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_BRACKETCLOSE:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case CHAR_SQUAREDBRACKETCLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_COLON:
		switch(c)
		{
		case CHAR_TABSPACE:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
		case CHAR_INVALID:
			return STATE_FINISHED;
		}
		break;
	case STATE_SEMICOLON:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NUMBER:
		case CHAR_ASSIGNMENT:
		case CHAR_QUESTIONMARK:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case SQS_STATE_LESSTHAN:
		switch(c)
		{
		case CHAR_ASSIGNMENT:
			return SQS_STATE_LESSOREQUAL;
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
		case CHAR_NUMBER:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case SQS_STATE_LESSOREQUAL:
		switch(c)
		{
		case CHAR_TABSPACE:
			return STATE_FINISHED;
		}
		break;
	case STATE_ASSIGNMENT:
		switch(c)
		{
		case CHAR_ASSIGNMENT:
			return STATE_EQUALS;
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_GREATERTHAN:
		switch(c)
		{
		case CHAR_ASSIGNMENT:
			return SQS_STATE_GREATEROREQUAL;
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
		case CHAR_NUMBER:
		case CHAR_LETTER:
			return STATE_FINISHED;
		}
		break;
	case SQS_STATE_GREATEROREQUAL:
		switch(c)
		{
		case CHAR_TABSPACE:
		case CHAR_NUMBER:
			return STATE_FINISHED;
		}
		break;
	case STATE_QUESTIONMARK:
		switch(c)
		{
		case CHAR_TABSPACE:
		case CHAR_NEGATION:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_AT:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_LETTER:
		switch(c)
		{
		case CHAR_DOT:
			return STATE_INVALID;
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_UNDERLINE:
			return STATE_LETTER;
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NEGATION:
		case CHAR_AND:
		case CHAR_BRACKETOPEN:
		case CHAR_BRACKETCLOSE:
		case SQS_CC_MULTIPLY:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case CHAR_COLON:
		case CHAR_SEMICOLON:
		case CHAR_ASSIGNMENT:
		case CHAR_GREATERTHAN:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_BRACEOPEN:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_SQUAREDBRACKETOPEN:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_INVERTEDCOMMA:
		case CHAR_BRACKETOPEN:
		case CHAR_MINUS:
		case CHAR_NUMBER:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
			return STATE_FINISHED;
		}
		break;
	case STATE_SQUAREDBRACKETCLOSE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NEGATION:
		case CHAR_BRACKETCLOSE:
		case CHAR_COMMA:
		case CHAR_SEMICOLON:
		case CHAR_ASSIGNMENT:
		case CHAR_GREATERTHAN:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_UNDERLINE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_LINEFEED:
			return STATE_INVALID;
		case CHAR_LETTER:
			return STATE_PRIVATEVARIABLE;
		}
		break;
	case STATE_BRACEOPEN:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
		case CHAR_LETTER:
		case CHAR_SQUAREDBRACKETOPEN:
		case CHAR_UNDERLINE:
		case CHAR_BRACEOPEN:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		case CHAR_DOT:
		case CHAR_INVALID:
			return STATE_UNCLOSEDBRACELITERAL;
		}
		break;
	case STATE_OR:
		switch(c)
		{
		case CHAR_NULLBYTE:
			return STATE_INVALID;
		case CHAR_OR:
			return STATE_LOGICALOR;
		}
		break;
	case STATE_BRACECLOSE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_BRACKETCLOSE:
		case CHAR_COMMA:
		case CHAR_SEMICOLON:
		case CHAR_LETTER:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		}
		break;
	case STATE_LOGICALOR:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_BRACKETOPEN:
		case CHAR_UNDERLINE:
			return STATE_FINISHED;
		}
		break;
	case STATE_TILDE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_NUMBER:
		case CHAR_LETTER:
			return STATE_FINISHED;
		}
		break;
	case STATE_PRIVATEVARIABLE:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_TABSPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGERETURN:
		case CHAR_NEGATION:
		case CHAR_BRACKETCLOSE:
		case CHAR_PLUS:
		case CHAR_COMMA:
		case CHAR_COLON:
		case CHAR_SEMICOLON:
		case CC_LESSTHAN:
		case CHAR_ASSIGNMENT:
		case CHAR_GREATERTHAN:
		case CHAR_SQUAREDBRACKETCLOSE:
		case CHAR_BRACECLOSE:
			return STATE_FINISHED;
		case CHAR_NUMBER:
		case CHAR_LETTER:
			return STATE_PRIVATEVARIABLE;
		}
		break;
	case STATE_INVALID:
		switch(c)
		{
		case CHAR_NULLBYTE:
		case CHAR_NUMBER:
		case CHAR_SEMICOLON:
		case CHAR_LETTER:
			return STATE_FINISHED;
		}
		break;
	}
	//this should never happen
	return STATE_ILLEGAL;
}

//Public Functions
CString CSQSLexer::GetCurrentTokenValue()
{
	return this->tokenValue;
}

void CSQSLexer::Init(const char *pSource)
{
	this->pSource = (char *)pSource;
	this->pSourceStart = pSource;
}

Token CSQSLexer::GetNextToken(bool ignore)
{
	char *pStart;
	TokenState state;
	TokenState acceptedState;
	char *pAcceptedPos;
	Token t;
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
			if(state == STATE_INVALID && c == CHAR_NULLBYTE)
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
	case TOKEN_TABSPACE:
	case TOKEN_CARRIAGERETURN:
		//ignore
		if(!ignore)
		{
			return t;
		}
		goto start;
	case TOKEN_SEMICOLON:
		if(this->pSource-1 != this->pSourceStart)
		{
			if(*(this->pSource-2) != '\n')
				return TOKEN_SEMICOLON;
		}

		while(*this->pSource != '\n' && *this->pSource != 0) this->pSource++;
		if(*this->pSource != 0)
		{
			this->pSource++;
		}
		this->tokenValue.Assign(pStart, (uint32)(this->pSource - pStart));
		//ignore
		if(!ignore)
		{
			return TOKEN_COMMENT;
		}
		goto start;
	case TOKEN_IDENTIFIER:
		return GetKeywordToken(this->tokenValue);
	case TOKEN_PRIVATEVARIABLE:
		if(this->tokenValue == "_this")
		{
			return TOKEN_THIS;
		}
		return TOKEN_PRIVATEVARIABLE;
	case TOKEN_END:
		this->pSource--; //again points to the 0 byte so that any following call (which should not happen) to this function again returns TOKEN_END
	default:
		return t;
	}
	return TOKEN_ILLEGAL;
}

uint32 CSQSLexer::GetCurrentOffset()
{
	return uint32(this->pSource - this->pSourceStart);
}