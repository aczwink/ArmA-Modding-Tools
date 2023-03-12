//Class Header
#include "CFilesTab.h"
//Local
#include "globals.h"

//Callbacks
bool CFilesTab::OnRightClick()
{
	POINT pt;
	CRect rc;
	int32 index;

	GetCursorPos(&pt);
	rc.left = pt.x;
	rc.top = pt.y;
	this->ScreenToClient(&rc);
	index = this->GetItemFromPos(rc.left, rc.top, TCHT_ONITEM);

	if(index != -1)
	{
		CMenu menu;

		menu.CreatePopup();
		menu.AppendString("Close", 1);

		switch(menu.TrackPopupMenu(ALIGN_LEFT, VALIGN_TOP, pt.x, pt.y, this->GetParent()))
		{
		case 1:
			this->Close(index);
			break;
		default:
			break;
		}
	}

	return true;
}

void CFilesTab::OnSelectionChanged()
{
	g_MainWindow.openFiles[this->GetSelection()].pTextView->Show(SW_SHOW);
}

bool CFilesTab::OnSelectionChanging()
{
	g_MainWindow.openFiles[this->GetSelection()].pTextView->Show(SW_HIDE);
	return true;
}

//Public Functions
void CFilesTab::Close(uint32 index)
{
	CMapIterator<uint32, SFile> it = g_MainWindow.openFiles.FindByKey(index);

	delete g_MainWindow.openFiles[index].pTextView;
	g_MainWindow.openFiles.Delete(index);
	this->DeleteItem(index);

	if(g_MainWindow.openFiles.GetLength())
	{
		if(it.IsAtEnd())
		{
			it--;
			this->Select(it.GetKey());
		}
		else
		{
			this->Select(it.GetKey());
		}
	}
	else
	{
		this->Select(-1);
	}
}