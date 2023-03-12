#pragma once
//SJCLib
#include <SJCLibWindows.h>
//Namespaces
using namespace SJCLibWindows;

class CFilesTab : public CTab
{
private:
	//Callbacks
	bool OnRightClick();
	void OnSelectionChanged();
	bool OnSelectionChanging();
public:
	//Functions
	void Close(uint32 index);
};