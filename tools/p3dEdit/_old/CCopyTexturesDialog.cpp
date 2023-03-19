//Class Header
#include "CCopyTexturesDialog.h"
//Local
#include "globals.h"

//CBrowseButton Message-callbacks
void CBrowseButton::OnClick()
{
	CFolderDialog dlg;
	LPITEMIDLIST result;

	dlg.Create("Choose directory", 0, this);
	if(result = dlg.Run())
	{
		pEdit->SetText(CFolderDialog::GetString(result));
	}
}

//CCopyButton Message-callbacks
void CCopyButton::OnClick()
{
	CCopyTexturesDialog *pDlg;
	int32 nSelectedItems;
	CString fromPath, toPath, dmgPattern, currentFile, currentPath, currentDmgFile;
	uint32 *pSelection;
	IP3dLod *pLod;
	CArray<CString> texturesToCopy;
	
	pDlg = (CCopyTexturesDialog *)this->GetParent();
	nSelectedItems = pDlg->texturePathsList.GetNumberOfSelectedItems();
	if(nSelectedItems == 0)
	{
		pDlg->MessageBox(L"Select a texturepath first", L"Error", MB_ICONWARNING);
		return;
	}

	fromPath = pDlg->fromPath.GetText();
	if(!IsDirectory(fromPath))
	{
		pDlg->MessageBox(L"\"Copy From\" path doesn't exist", L"Error", MB_ICONWARNING);
		return;
	}

	toPath = pDlg->toPath.GetText();
	if(!IsDirectory(toPath))
	{
		pDlg->MessageBox(L"\"Copy To\" path doesn't exist", L"Error", MB_ICONWARNING);
		return;
	}

	pSelection = (uint32 *)malloc(nSelectedItems * sizeof(*pSelection));
	pDlg->texturePathsList.GetSelectedItems(pSelection);
	dmgPattern = pDlg->dmgTexturesPattern.GetText();
	
	repeat(g_pDataMgr->GetNumberOfLods(), i)
	{
		pLod = g_pDataMgr->GetLod(i);

		repeat(pLod->GetNumberOfTextures(), j)
		{
			currentFile = pLod->GetTexture(j);
			currentPath = GetFilePath(currentFile).ToCString();
			
			repeat((uint32)nSelectedItems, k)
			{
				if(currentPath == pDlg->texturePathsList.GetItemText(pSelection[k]))
				{
					currentDmgFile = dmgPattern;
					currentDmgFile.Replace("name", GetFileName(currentFile).ToCString());
					currentDmgFile.Replace("ext", GetFileExtension(currentFile));
					currentDmgFile = currentPath + "\\" + currentDmgFile;
					
					texturesToCopy.Push(currentFile);
				}
			}
		}
	}
	
	repeat(texturesToCopy.GetNoOfElements(), i)
	{
		CreateDirectoryAll(GetFilePath(toPath + "\\" + texturesToCopy[i]));
		
		if(!CopyFile((fromPath + "\\" + texturesToCopy[i]).GetC_Str(), (toPath + "\\" + texturesToCopy[i]).GetC_Str(), FALSE))
		{
			pDlg->MessageBox(L"Couldn't copy file: " + texturesToCopy[i], L"Error", MB_ICONERROR);
		}
	}

	free(pSelection);

	pDlg->MessageBox(L"Done!", L"Task completed", MB_ICONINFORMATION);
}

//Message-callbacks
void CCopyTexturesDialog::OnInit()
{
	CFiniteSet<CString> textures;
	
	this->texturePathsList.Create(LBS_STANDARD | LBS_MULTIPLESEL, 0, *this);
	this->texturePathsList.SetPos(this->DialogToScreenUnits(CRect(5, 5, 245, 120)));
	this->texturePathsList.SetFont(g_DefaultFont);

	this->dmgTexturesPatternInfo.Create("Damaged Textures Pattern (use 'name' and 'ext' as placeholders):", 0, *this);
	this->dmgTexturesPatternInfo.SetPos(this->DialogToScreenUnits(CRect().Init(5, 120, 240, 140)));
	this->dmgTexturesPatternInfo.SetFont(g_DefaultFont);

	this->dmgTexturesPattern.Create(0, *this);
	this->dmgTexturesPattern.SetPos(this->DialogToScreenUnits(CRect().Init(5, 130, 95, 14)));
	this->dmgTexturesPattern.SetText("name_d.ext");
	this->dmgTexturesPattern.SetFont(g_DefaultFont);

	this->fromPath.Create(ES_AUTOHSCROLL, *this);
	this->fromPath.SetPos(this->DialogToScreenUnits(CRect().Init(5, 150, 95, 14)));
	this->fromPath.SetCueBanner(L"Copy from...");
	this->fromPath.SetFont(g_DefaultFont);

	this->browseFrom.Create("Browse", 0, *this);
	this->browseFrom.SetPos(this->DialogToScreenUnits(CRect().Init(105, 150, 50, 14)));
	this->browseFrom.pEdit = &this->fromPath;
	this->browseFrom.SetFont(g_DefaultFont);

	this->toPath.Create(ES_AUTOHSCROLL, *this);
	this->toPath.SetPos(this->DialogToScreenUnits(CRect().Init(5, 165, 95, 14)));
	this->toPath.SetCueBanner(L"Copy to...");
	this->toPath.SetFont(g_DefaultFont);

	this->browseTo.Create("Browse", 0, *this);
	this->browseTo.SetPos(this->DialogToScreenUnits(CRect().Init(105, 165, 50, 14)));
	this->browseTo.pEdit = &this->toPath;
	this->browseTo.SetFont(g_DefaultFont);

	this->copy.Create("Copy", 0, *this);
	this->copy.SetPos(this->DialogToScreenUnits(CRect().Init(5, 185, 50, 14)));
	this->copy.SetFont(g_DefaultFont);

	this->close.Create("Close", 0, *this);
	this->close.SetPos(this->DialogToScreenUnits(CRect().Init(60, 185, 50, 14)));
	this->close.SetFont(g_DefaultFont);
	
	repeat(g_pDataMgr->GetNumberOfLods(), i)
	{
		g_pDataMgr->GetLod(i)->GetTextures(textures);
	}
	foreach(it, textures)
	{
		if(it.GetValue().IsEmpty())
			continue;
		this->texturePaths.Insert(GetFilePath(it.GetValue()).ToCString());
	}
	foreach(it, texturePaths)
	{
		this->texturePathsList.AddItem(it.GetValue());
	}
}

//Public Functions
void CCopyTexturesDialog::Create(const CWindow *pParentWindow)
{
	CDialog::Create("Copy Textures", DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU, 0, 0, 250, 205, pParentWindow, g_DefaultFont);
}