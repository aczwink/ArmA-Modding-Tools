//SJC
#include <SJCWinLib.h>
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

class CBrowseButton : public CButton
{
private:
	//Message-callbacks
	void OnClick();
public:
	//Variables
	CEdit *pEdit;
};

class CCopyButton : public CButton
{
private:
	//Message-callbacks
	void OnClick();
};

class CCopyTexturesDialog : public CDialog
{
	friend class CCopyButton;
private:
	//Variables
	CFiniteSet<CString> texturePaths;
	//Controls
	CListBox texturePathsList;
	CStatic dmgTexturesPatternInfo;
	CEdit dmgTexturesPattern;
	CEdit fromPath;
	CBrowseButton browseFrom;
	CEdit toPath;
	CBrowseButton browseTo;
	CCopyButton copy;
	CCommonButtonClose close;
	//Message-callbacks
	void OnInit();
public:
	//Functions
	void Create(const CWindow *pParentWindow);
};