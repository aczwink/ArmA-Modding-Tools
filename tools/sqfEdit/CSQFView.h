#pragma once
//SJCLib
#include <SJCLibWindows.h>
//BISLib
#include <BISLibrary.h>
//Namespaces
using namespace SJCLib;
using namespace SJCLibWindows;
using namespace BISLibrary;

struct SSQFTokenInfo
{
	uint32 from;
	uint32 to;
	SQFToken token;
};

class CSQFView : public CMSTextView
{
private:
	//Variables
	CArray<SSQFTokenInfo> tokenInfos;
	//Callbacks
	void OnChangeText(uint32 pos, int32 delta);
	void OnDrawText(STextViewDrawInfo *pDrawInfo);
	//Functions
	bool GetToken(uint32 from, SQFToken &t, uint32 &to);
};