//SJC
#include <SJCWinLib.h>
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

class CTextureList : public CListBox
{
private:
	//Message-callbacks
	void OnChangeSelection();
};

class CSetButton : public CButton
{
private:
	//Message-callbacks
	void OnClick();
};

class CTextureReplaceDialog : public CDialog
{
	friend class CTextureList;
	friend class CSetButton;
private:
	//Variables
	CFiniteSet<CString> textures;
	//Controls
	CTextureList list;
	CEdit newTexture;
	CSetButton set;
	CCommonButtonClose close;
	//Message-callbacks
	void OnInit();
	//Functions
	void UpdateList();
public:
	//Functions
	void Create(const CWindow *pParentWindow);
};