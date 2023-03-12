/*
//Local
#include "CFilesTab.h"
//Namespaces
using namespace SJCLib;

//Structs
struct SFile
{
	CString fileName;
	CString filePath;
	bool isSaved;
	CTextView *pTextView;
};

class CMainWindow : public CWindow
{
private:
	//Variables
	CLinkedList<SFile> openedFiles;
	uint32 newIndex;
	//Message-callbacks
	void OnDestroy();
	void OnMenuCommand(uint32 menuItemId);
	void OnSize(WPARAM resizingType, uint32 newWidth, uint32 newHeight);
	//Functions
	void BuildMenu();
	void SetFileUnsaved(uint32 index);
public:
	//Constructor
	CMainWindow();
};*/