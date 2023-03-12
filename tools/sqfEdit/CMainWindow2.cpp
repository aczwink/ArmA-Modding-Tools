//Class Header
#include "CMainWindow.h"
//SJCLib
#include <SJCLibFile.h>
//Local
#include "CSQFView.h"
#include "globals.h"
#include "resource.h"
//Namespaces
using namespace SJCLib;
using namespace SJCLibFile;
using namespace SJCLibString;

//Callbacks
void CMainWindow::OnAcceleratorCommand(uint32 acceleratorCmdId)
{
	switch(acceleratorCmdId)
	{
	case ID_FILE_OPEN:
		this->PostMessageA(MsgMenuCommand(ID_FILE_OPEN));
		//this->OnMenuCommand(ID_FILE_OPEN);
		break;
	case ID_FILE_SAVE:
		this->OnMenuCommand(ID_FILE_SAVE);
		break;
	}
}

void CMainWindow::OnCreate(CREATESTRUCT *pCreateStruct)
{
	CMenu menu;
	CSmartPointer<CMenu> pSubMenu;

	
	this->newIndex = 0;

	//Build Menu
	pSubMenu->AppendString("Open\tCTRL+O", ID_FILE_OPEN);
	pSubMenu->AppendString("Close", ID_FILE_CLOSE);
	pSubMenu->AppendString("Save\tCTRL+S", ID_FILE_SAVE);
	pSubMenu->AppendString("Save as", ID_FILE_SAVEAS);
	pSubMenu->AppendSeperator();
	pSubMenu->AppendString("Exit", ID_FILE_EXIT);

	//?
	pSubMenu = menu.AddPopup();
	pSubMenu->AppendString("About", ID_QUESTIONMARK_ABOUT);
	menu.AppendSubMenu("?", pSubMenu);
}

void CMainWindow::OnMenuCommand(uint32 menuId)
{
	switch(menuId)
	{
	case ID_FILE_OPEN:
		{
			CCommonItemOpenDialog dlg;
			COMDLG_FILTERSPEC filespecs[] =
			{
				{L"SQS file", L"*.sqs"},
				{L"SQF file", L"*.sqf"},
				{L"SQF/SQS files", L"*.sqf;*.sqs"}
			};
			
			dlg.Create(this);
			dlg.SetFileTypes(sizeof(filespecs)/sizeof(filespecs[0]), filespecs);
			dlg.SetFileTypeIndex(3);

			if(dlg.Run())
			{
				this->OpenFile(dlg.GetResult());
			}
		}
		break;
	case ID_FILE_CLOSE:
		{
			int32 selection;
			
			selection = this->tabs.GetSelection();
			if(selection != -1)
			{
				this->tabs.Close(selection);
			}
		}
		break;
	case ID_FILE_SAVE:
		{
			int32 selection;

			selection = this->tabs.GetSelection();

			if(selection != -1 && !this->openFiles[selection].isSaved)
			{
				this->SaveFile(selection);
			}
		}
		break;
	case ID_FILE_SAVEAS:
		{
			int32 selection;

			selection = this->tabs.GetSelection();

			if(selection != -1)
			{
				this->openFiles[selection].filePath = "";
				this->SaveFile(selection);
			}
		}
		break;
	case ID_FILE_EXIT:
		this->Destroy();
		break;
	case ID_QUESTIONMARK_ABOUT:
		this->MessageBoxA((CString)APPLICATION_NAME + " by " + APPLICATION_CREATOR + "\nVersion: " + APPLICATION_VERSION_STRING + "\nBuilddate: " + BUILDDATE_STRING, "About", MB_ICONINFORMATION);
		break;
	}
}

void CMainWindow::OnUserMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_USER_OPENFILE)
	{
		CString path;
		CBIFStream file;

		path = GetFilePath(g_Module.GetFileName());
		file.Open(path + "\\_" + CString::FromUInt32((uint32)lParam));
		this->OpenFile(file.ReadString());
		file.Close();
		DeleteFile(file.GetFilename().c_str());
		this->BringToForeground();
	}
}

//Private Functions
void CMainWindow::SaveFile(uint32 index)
{
	COFStream file;

	if(this->openFiles[index].filePath == "")
	{
		CCommonItemSaveDialog dlg;
		CString result;
		COMDLG_FILTERSPEC filespecs[] =
		{
			{L"SQS file", L"*.sqs"},
			{L"SQF file", L"*.sqf"}
		};

		dlg.Create(this);
		dlg.SetFileTypes(sizeof(filespecs)/sizeof(filespecs[0]), filespecs);

		if(!dlg.Run())
		{
			return;
		}

		result = dlg.GetResult();
		this->openFiles[index].fileName = GetFileName(result) + '.' + GetFileExtension(result);
		this->openFiles[index].filePath = GetFilePath(result);
		this->tabs.SetText(index, this->openFiles[index].fileName);
	}

	file.Open(this->openFiles[index].filePath + "\\" + this->openFiles[index].fileName);
	file.WriteBytes(this->openFiles[index].pTextView->GetTextOutput().c_str(), this->openFiles[index].pTextView->GetTextOutput().GetLength());
	this->tabs.SetText(index, this->openFiles[index].fileName);
	this->openFiles[index].isSaved = true;
	this->tabs.Select(index, true);
	this->openFiles[index].pTextView->Focus();
}

//Public Functions
void CMainWindow::OpenFile(CString filename)
{
	CString ext;
	CBIFStream input;
	CRect rcDisplay;
	SFile file;
	char *pBuffer;
	uint32 length;
	int32 index;

	cout << filename << endl;

	ext = GetFileExtension(filename);
	if(ext == "sqs")
	{
		file.fileName = GetFileName(filename) + ".sqs";
		file.pTextView = new CSQSView;
	}
	else if(ext == "sqf")
	{
		file.fileName = GetFileName(filename) + ".sqf";
		file.pTextView = new CSQFView;
	}
	else
	{
		ASSERT(false);
		return;
	}
	file.filePath = GetFilePath(filename);
	file.isSaved = true;
	index = this->newIndex++;
	
	input.Open(filename);
	length = input.GetRemainingBytes();
	pBuffer = (char *)malloc(length);
	input.ReadBytes(pBuffer, length);
	
	index = this->tabs.InsertItem(index, file.fileName);
	this->openFiles[index] = file;
	this->tabs.GetDisplayArea(&rcDisplay);
	file.pTextView->Create(0, (uint16)rcDisplay.left, (uint16)rcDisplay.top, rcDisplay.GetWidth(), rcDisplay.GetHeight(), &this->tabs);
	file.pTextView->SetFont(&g_CodeFont);
	file.pTextView->SetText(CString(pBuffer, length));
	this->openFiles[index].isSaved = true;
	this->tabs.SetText(index, file.fileName);
	this->tabs.Select(index, true);
	
	free(pBuffer);
}