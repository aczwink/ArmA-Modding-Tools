//Class Header
#include "CSQSView.h"
//Local
#include "globals.h"
//Namespaces
using namespace SJCLibString;

//Callbacks
void CSQSView::OnChangeText(uint32 pos, int32 delta)
{
	const CString &text = this->GetText();
	CSQSLexer lexer;
	Token t;
	STokenInfo tokenInfo;
	int32 index;

	this->tokenInfos.Release();	
	lexer.Init(text.c_str());
	while((t = lexer.GetNextToken(false)) != TOKEN_END)
	{
		tokenInfo.to = lexer.GetCurrentOffset();
		tokenInfo.from = tokenInfo.to - lexer.GetCurrentTokenValue().GetLength();
		tokenInfo.token = t;

		this->tokenInfos.Push(tokenInfo);
	}

	index = g_MainWindow.tabs.GetSelection();
	if(g_MainWindow.openFiles[index].isSaved)
	{
		g_MainWindow.openFiles[index].isSaved = false;
		g_MainWindow.SetFileUnsaved(index);
	}

	this->UpdateScrollBars();
}

void CSQSView::OnDrawText(STextViewDrawInfo *pDrawInfo)
{
	Token t = TOKEN_ILLEGAL;
	
	ASSERT(this->GetToken(pDrawInfo->from, t, pDrawInfo->to));
	pDrawInfo->backgroundColor = CColor::GetSystemColor(COLOR_WINDOW);
	pDrawInfo->textColor = CColor::GetSystemColor(COLOR_WINDOWTEXT);
	
	switch(t)
	{
	case TOKEN_ASSIGNMENT:
	case SQS_TOKEN_LESSTHAN:
	case SQS_TOKEN_LESSOREQUAL:
	case TOKEN_GREATERTHAN:
	case SQS_TOKEN_GREATEROREQUAL:
	case TOKEN_QUESTIONMARK:
	case TOKEN_WAITFORCONDITION:
	case TOKEN_NEGATION:
	case TOKEN_EQUALS:
	case TOKEN_NOTEQUALS:
	case TOKEN_WAITFORTIME:
	case SQS_TOKEN_LOGICALAND:
	case TOKEN_BRACKETOPEN:
	case TOKEN_BRACKETCLOSE:
	case SQS_TOKEN_MULTIPLY:
	case TOKEN_PLUS:
	case SQS_TOKEN_MINUS:
	case SQS_TOKEN_SLASH:
	case TOKEN_COMMA:
	case TOKEN_COLON:
	case TOKEN_SEMICOLON:
	case TOKEN_SQUAREDBRACKETOPEN:
	case TOKEN_SQUAREDBRACKETCLOSE:
	case TOKEN_BRACEOPEN:
	case TOKEN_BRACECLOSE:
	case TOKEN_LOGICALOR:
	case TOKEN_DELAY:
		pDrawInfo->textColor.Set(0, 0, 128); //Navy
		break;
	case TOKEN_LITERAL:
		pDrawInfo->textColor.Set(128, 0, 0); //Maroon
		break;
	case TOKEN_LABEL:
		pDrawInfo->textColor.Set(128, 128, 128); //Gray
		break;
	case TOKEN_INT:
	case TOKEN_FLOAT:
		pDrawInfo->textColor.Set(255, 0, 0); //Red
		break;
	case TOKEN_COMMENT:
		pDrawInfo->textColor.Set(0, 128, 0); //Green
		break;
	case TOKEN_IDENTIFIER:
		{
			const CString &text = this->GetText().SubString(pDrawInfo->from, pDrawInfo->to - pDrawInfo->from);
			repeat(sizeof(g_SQSCommands)/sizeof(g_SQSCommands[0]), i)
			{
				if(g_SQSCommands[i] == text)
				{
					pDrawInfo->textColor.Set(0, 0, 255); //Blue
					break;
				}
			}
		}
		break;
	case TOKEN_PRIVATEVARIABLE:
		pDrawInfo->textColor.Set(128, 128, 0); //Olive
		break;
	case TOKEN_THIS:
		pDrawInfo->textColor.Set(0, 128, 128); //Teal
		break;
	}
}

//Private Functions
bool CSQSView::GetToken(uint32 from, Token &t, uint32 &to)
{
	repeat(this->tokenInfos.GetLength(), i)
	{
		if(this->tokenInfos[i].from == from /*&& to < this->tokenInfos[i].to*/)
		{
			t = this->tokenInfos[i].token;
			to = this->tokenInfos[i].to;
			return true;
		}
		else if(this->tokenInfos[i].from > from)
		{
			t = this->tokenInfos[i-1].token;
			to = this->tokenInfos[i-1].to;
			return true;
		}
	}

	return false;
}