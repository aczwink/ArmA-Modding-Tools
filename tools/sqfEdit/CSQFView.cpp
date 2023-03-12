//Class Header
#include "CSQFView.h"
//Local
#include "globals.h"
//Namespaces
using namespace SJCLibString;

//Callbacks
void CSQFView::OnChangeText(uint32 pos, int32 delta)
{
	const CString &text = this->GetText();
	CSQFLexer lexer;
	SQFToken t;
	SSQFTokenInfo tokenInfo;
	int32 index;

	this->tokenInfos.Release();
	lexer.Init(text.c_str());
	while((t = lexer.GetNextToken(false)) != SQF_TOKEN_END)
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

void CSQFView::OnDrawText(STextViewDrawInfo *pDrawInfo)
{
	SQFToken t = SQF_TOKEN_ILLEGAL;
	
	ASSERT(this->GetToken(pDrawInfo->from, t, pDrawInfo->to));
	pDrawInfo->backgroundColor = CColor::GetSystemColor(COLOR_WINDOW);
	pDrawInfo->textColor = CColor::GetSystemColor(COLOR_WINDOWTEXT);
	
	switch(t)
	{
	case SQF_TOKEN_LITERAL:
		pDrawInfo->textColor.Set(128, 0, 0); //Maroon
		break;
	case SQF_TOKEN_SINGLELINECOMMENT:
	case SQF_TOKEN_MULTILINECOMMENT:
		pDrawInfo->textColor.Set(0, 128, 0); //Green
		break;
	case SQF_TOKEN_NUMBER:
	case SQF_TOKEN_FLOAT:
		pDrawInfo->textColor.Set(255, 0, 0); //Red
		break;
	case SQF_TOKEN_LOGICALAND:
	case SQF_TOKEN_BRACKETOPEN:
	case SQF_TOKEN_BRACKETCLOSE:
	case SQF_TOKEN_MULTIPLY:
	case SQF_TOKEN_PLUS:
	case SQF_TOKEN_COMMA:
	case SQF_TOKEN_SEMICOLON:
	case SQF_TOKEN_ASSIGNMENT:
	case SQF_TOKEN_EQUALS:
	case SQF_TOKEN_NOTEQUALS:
	case SQF_TOKEN_LESSTHAN:
	case SQF_TOKEN_GREATERTHAN:
	case SQF_TOKEN_SQUAREDBRACKETOPEN:
	case SQF_TOKEN_SQUAREDBRACKETCLOSE:
	case SQF_TOKEN_BRACEOPEN:
	case SQF_TOKEN_BRACECLOSE:
		pDrawInfo->textColor.Set(0, 0, 128); //Navy
		break;
	case SQF_TOKEN_PRIVATEVARIABLE:
		pDrawInfo->textColor.Set(128, 128, 0); //Olive
		break;
	case SQF_TOKEN_IDENTIFIER:
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
	}
	/*
	case TOKEN_LABEL:
		pDrawInfo->textColor.Set(128, 128, 128); //Gray
		break;
	case TOKEN_THIS:
		pDrawInfo->textColor.Set(0, 128, 128); //Teal
		break;
	}*/
}

//Private Functions
bool CSQFView::GetToken(uint32 from, SQFToken &t, uint32 &to)
{
	uint32 i;
	
	for(i = 0; i < this->tokenInfos.GetLength(); i++)
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

	t = this->tokenInfos[i-1].token;
	to = this->tokenInfos[i-1].to;
	return true;
}