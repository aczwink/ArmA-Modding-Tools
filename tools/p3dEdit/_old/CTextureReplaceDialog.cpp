//Class Header
#include "CTextureReplaceDialog.h"
//Local
#include "globals.h"

//CTextureList Message-callbacks
void CTextureList::OnChangeSelection()
{
	CTextureReplaceDialog *pDlg;
	int32 selection;

	pDlg = (CTextureReplaceDialog *)this->GetParent();
	selection = this->GetCurrentSelection();
	if(selection != LB_ERR)
	{
		pDlg->newTexture.SetText(this->GetItemText(selection));
	}
}

//CSetButton Message-callbacks
void CSetButton::OnClick()
{
	CTextureReplaceDialog *pDlg;
	int32 selection;

	pDlg = (CTextureReplaceDialog *)this->GetParent();
	selection = pDlg->list.GetCurrentSelection();
	if(selection != LB_ERR)
	{
		CString oldTexture, newTexture;
		IP3dLod *pLod;

		oldTexture = pDlg->list.GetItemText(selection);
		newTexture = pDlg->newTexture.GetText();
		
		repeat(g_pDataMgr->GetNumberOfLods(), i)
		{
			pLod = g_pDataMgr->GetLod(i);

			repeat(pLod->GetNumberOfTextures(), j)
			{
				if(pLod->GetTexture(j) == oldTexture)
				{
					pLod->SetTexture(j, newTexture);
				}
			}
		}
		
		pDlg->UpdateList();
	}
}

//Message-callbacks
void CTextureReplaceDialog::OnInit()
{
	this->list.Create(LBS_STANDARD, 0, *this);
	this->list.SetPos(this->DialogToScreenUnits(CRect(5, 5, 245, 120)));
	this->list.SetFont(g_DefaultFont);
	
	this->newTexture.Create(0, *this);
	this->newTexture.SetPos(this->DialogToScreenUnits(CRect().Init(5, 120, 100, 14)));
	this->newTexture.SetFont(g_DefaultFont);

	this->set.Create("Set", 0, *this);
	this->set.SetPos(this->DialogToScreenUnits(CRect().Init(110, 120, 50, 14)));
	this->set.SetFont(g_DefaultFont);

	this->close.Create("Close", 0, *this);
	this->close.SetPos(this->DialogToScreenUnits(CRect().Init(5, 160, 50, 14)));
	this->close.SetFont(g_DefaultFont);
	
	this->UpdateList();
}

//Private Functions
void CTextureReplaceDialog::UpdateList()
{
	IP3dLod *pLod;

	this->textures.Release();

	repeat(g_pDataMgr->GetNumberOfLods(), i)
	{
		pLod = g_pDataMgr->GetLod(i);
		pLod->GetTextures(this->textures);
	}
	
	this->list.Clear();
	
	foreach(it, this->textures)
	{
		this->list.AddItem(it.GetValue());
	}
}

//Public Functions
void CTextureReplaceDialog::Create(const CWindow *pParentWindow)
{
	CDialog::Create("Replace Textures", WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER, 0, 0, 250, 180, pParentWindow, g_DefaultFont);
}