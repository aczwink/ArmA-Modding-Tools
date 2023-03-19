//Class Header
#include "CTexturepathsReplaceDialog.h"
//Local
#include "globals.h"

//CTexturepathList Message-callbacks
void CTexturepathList::OnChangeSelection()
{
	CTexturepathsReplaceDialog *pDlg;
	int32 selection;

	pDlg = (CTexturepathsReplaceDialog *)this->GetParent();
	selection = pDlg->pathsList.GetCurrentSelection();
	if(selection != LB_ERR)
	{
		pDlg->newPath.SetText(this->GetItemText(selection));
	}
}

//CSetTexturepathButton Message-callbacks
void CSetTexturepathButton::OnClick()
{
	CTexturepathsReplaceDialog *pDlg;
	int32 selection;
	IP3dLod *pLod;

	pDlg = (CTexturepathsReplaceDialog *)this->GetParent();
	selection = pDlg->pathsList.GetCurrentSelection();
	if(selection != LB_ERR)
	{
		CString buffer, newPath;
		int32 pos;

		newPath = pDlg->newPath.GetText();

		repeat(g_pDataMgr->GetNumberOfLods(), i)
		{
			pLod = g_pDataMgr->GetLod(i);
			repeat(pLod->GetNumberOfTextures(), j)
			{
				buffer = pLod->GetTexture(j);
				pos = buffer.FindReverse('\\');
				if(pos == STRING_NOMATCH)
				{
					pos = 0;
				}
				if(buffer.SubString(0, pos) == pDlg->pathsList.GetItemText(selection))
				{
					buffer = newPath + buffer.SubString(pos, buffer.GetLength()-pos);
					pLod->SetTexture(j, buffer);
				}
			}
		}
		
		pDlg->UpdateList();
	}
}

//Message-callbacks
void CTexturepathsReplaceDialog::OnInit()
{
	this->pathsList.Create(LBS_STANDARD, 0, *this);
	this->pathsList.SetPos(this->DialogToScreenUnits(CRect(5, 5, 245, 120)));
	this->pathsList.SetFont(g_DefaultFont);

	this->newPath.Create(0, *this);
	this->newPath.SetPos(this->DialogToScreenUnits(CRect().Init(5, 120, 95, 14)));
	this->newPath.SetFont(g_DefaultFont);

	this->set.Create("Set", 0, *this);
	this->set.SetPos(this->DialogToScreenUnits(CRect().Init(105, 120, 50, 14)));
	this->set.SetFont(g_DefaultFont);

	this->close.Create("Close", 0, *this);
	this->close.SetPos(this->DialogToScreenUnits(CRect().Init(5, 140, 50, 14)));
	this->close.SetFont(g_DefaultFont);

	this->UpdateList();
}

//Private Functions
void CTexturepathsReplaceDialog::UpdateList()
{
	CString buffer;
	int32 pos;
	IP3dLod *pLod;

	this->texturePaths.Release();

	repeat(g_pDataMgr->GetNumberOfLods(), i)
	{
		pLod = g_pDataMgr->GetLod(i);
		repeat(pLod->GetNumberOfTextures(), j)
		{
			buffer = pLod->GetTexture(j);
			pos = buffer.FindReverse('\\');
			if(pos == STRING_NOMATCH)
			{
				pos = 0;
			}
			buffer = buffer.SubString(0, pos);
			this->texturePaths.Insert(buffer);
		}
	}
	
	this->pathsList.Clear();

	foreach(it, this->texturePaths)
	{
		this->pathsList.AddItem(it.GetValue());
	}
}

//Public Functions
void CTexturepathsReplaceDialog::Create(const CWindow *pParentWindow)
{
	CDialog::Create("Replace Texturepaths", WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER, 0, 0, 250, 160, pParentWindow, g_DefaultFont);
}