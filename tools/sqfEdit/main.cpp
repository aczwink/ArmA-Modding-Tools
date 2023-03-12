//SJC Libs
#include <SJCWinLib.h>
//Local
#include "CMainFrame.h"
#include "resource.h"
//Namespaces
using namespace SJCLib;
using namespace SJCWinLib;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	CApplication app;
	CMainFrame mainFrame;

#ifdef _DEBUG
	AllocateConsole();
	CWindow::GetConsoleWindow()->Move(0, 0, 500, 200);
	SetConsoleTitle("Debug Console");
#endif
	
	mainFrame.Create(APPLICATION_NAME, STYLE_OUTERPADDING, STYLE_OUTERPADDING, (GetSystemMetrics(SM_CXSCREEN) - 2*STYLE_OUTERPADDING), (GetSystemMetrics(SM_CYSCREEN) - 2*STYLE_OUTERPADDING));	
	mainFrame.Show(nCmdShow);
	mainFrame.Update();
	mainFrame.Move(100, 100, 800, 800);
	//mainFrame.Show(SW_MAXIMIZE);
	
	return app.Run();
}

/*
//Local
#include "CMainWindow.h"

//Global Variables
CFont g_CodeFont;
CFont g_DefaultFont;
*/