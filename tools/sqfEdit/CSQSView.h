#pragma once
//SJCLib
#include <SJCLibWindows.h>
//BISLib
#include <BISLibrary.h>
//Namespaces
using namespace SJCLib;
using namespace SJCLibWindows;
using namespace BISLibrary;

struct STokenInfo
{
	uint32 from;
	uint32 to;
	Token token;
};

class CSQSView : public CMSTextView
{
private:
	//Variables
	CArray<STokenInfo> tokenInfos;
	//Callbacks
	void OnChangeText(uint32 pos, int32 delta);
	void OnDrawText(STextViewDrawInfo *pDrawInfo);
	//Functions
	bool GetToken(uint32 from, Token &t, uint32 &to);
};