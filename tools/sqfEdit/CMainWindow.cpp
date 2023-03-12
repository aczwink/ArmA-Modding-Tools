/*//Class Header
#include "CMainWindow.h"
//Local
#include "globals.h"
#include "resource.h"
//Namespaces
using namespace SJCWinLib;

//Constructor
CMainWindow::CMainWindow()
{
	this->newIndex = 0;
}

//Message-callbacks
bool CMainWindow::OnCreate()
{
	g_DefaultFont.CreateSystemFont(DEFAULT_GUI_FONT);
	g_CodeFont.Create(14, 0, 0, 0, FW_NORMAL, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	
	this->BuildMenu();

	this->tabs.SetFont(g_DefaultFont);

	return true;
}

void CMainWindow::OnMenuCommand(uint32 menuItemId)
{
	switch(menuItemId)
	{
	case ID_FILE_NEWSQS:
		{
			CRect rcDisplay;
			SFile file;
			uint32 index;
			
			file.fileName = "new sqs file";
			file.filePath = "";
			file.isSaved = false;
			file.pTextView = new CTextView;
			//file.pTextView = new CSQSView;
			index = this->newIndex++;
			
			index = this->tabs.InsertItem(index, file.fileName);
			this->openedFiles[index] = file;
			this->SetFileUnsaved(index);
			this->tabs.GetDisplayArea(&rcDisplay);
			file.pTextView->Create(0, 0, rcDisplay.left, rcDisplay.top, rcDisplay.GetWidth(), rcDisplay.GetHeight(), this->tabs);
			file.pTextView->SetFont(g_CodeFont);
			this->tabs.Select(index, true);

			file.pTextView->SetText("blablabla\r\newnhfsfos\r\n\r\nqwe\nase jhd\tp\tüüü");
		}
		break;
	}
}

//Private Functions
void CMainWindow::BuildMenu()
{
	CMenu menu, subMenu;

	menu.Create();
	//File
	subMenu.CreatePopup();
	subMenu.AppendItem("New SQS", ID_FILE_NEWSQS);
	menu.AppendSubMenu("File", subMenu);

	this->SetMenu(menu);
}

void CMainWindow::SetFileUnsaved(uint32 index)
{
	this->tabs.SetText(index, this->openedFiles[index].fileName + "*");
}*/