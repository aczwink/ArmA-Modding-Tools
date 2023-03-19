//SJC
#include <SJCWinLib.h>
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

class CTexturepathList : public CListBox
{
private:
	//Callbacks
	void OnChangeSelection();
};

class CSetTexturepathButton : public CButton
{
private:
	//Callbacks
	void OnClick();
};

class CTexturepathsReplaceDialog : public CDialog
{
	friend class CTexturepathList;
	friend class CSetTexturepathButton;
private:
	//Variables
	CFiniteSet<CString> texturePaths;
	//Controls
	CTexturepathList pathsList;
	CEdit newPath;
	CSetTexturepathButton set;
	CCommonButtonClose close;
	//Message-callbacks
	void OnInit();
	//Functions
	void UpdateList();
public:
	//Functions
	void Create(const CWindow *pParentWindow);
};