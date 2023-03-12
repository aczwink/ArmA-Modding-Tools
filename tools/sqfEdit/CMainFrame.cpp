//Class Header
#include "CMainFrame.h"
//Local
#include "CTextView.h"
#include "CTextDocument.h"

//Callbacks
bool CMainFrame::OnCreate()
{
	this->tabs.Create(*this);

	SJCLib::CPointer<CWindow> pt;
	CTextDocument *pDoc = new CTextDocument;
	CTextView *pView = new CTextView(pDoc);
	SJCLib::CFileInputStream ifs;
	ifs.Open("1.sqs");
	//pDoc->SetText("bla\nbli\nblu\nnur der mann im mond sch\naut zu");
	pDoc->SetText(ifs.ReadString());
	pView->Create(this->tabs);
	pt.Attach(pView);
	
	this->tabs.AddTab("bla", pt);

	return true;
}

void CMainFrame::OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight)
{
	CRect rcClient;
	
	this->GetClientRect(rcClient);
	
	this->tabs.SetPos(rcClient, SWP_NOZORDER);
}