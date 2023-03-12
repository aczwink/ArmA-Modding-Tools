//Definitions
#define WIN32_LEAN_AND_MEAN
//Global
#include <ctime>
#include <Windows.h>
#include <TlHelp32.h>
//SJCLib
#include <SJCLibFile.h>
//Local
#include "globals.h"
#include "resource.h"
//Namespaces
using namespace SJCLibFile;
//Global Variables
CFont g_CodeFont;
CFont g_DefaultFont;
CMainWindow g_MainWindow;
CModule g_Module;

void OpenFile(CString cmdLine, CMainWindow *pWnd)
{
	if(!cmdLine.IsEmpty())
	{
		CString path;
		uint32 value;
		COFStream file;

		if(cmdLine[0] == '"')
			cmdLine = cmdLine.SubString(1, cmdLine.GetLength()-2);

		path = GetFilePath(g_Module.GetFileName());
		srand((uint32)time(NULL));
		value = rand();
		file.Open(path + "\\_" + CString::FromUInt32(value));
		file.WriteString(cmdLine);
		file.WriteByte(0);
		file.Close();

		pWnd->SendMessage(WM_USER_OPENFILE, 0, value);
	}
}

bool IsProcessRunning(CString cmdLine)
{
	HANDLE hSnap;
	PROCESSENTRY32 pe;

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnap, &pe);

	while(Process32Next(hSnap, &pe))
	{
		if(pe.szExeFile == (CString)APPLICATION_ORIGINALFILENAME)
		{
			if(pe.th32ProcessID != GetCurrentProcessId())
			{
				OpenFile(cmdLine, (CMainWindow *)CWindow::FindWindowA(APPLICATION_MAINWNDCLASS, CString()));
				CloseHandle(hSnap);
				return true;
			}
		}
	}

	CloseHandle(hSnap);

	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HACCEL hAccel;
	ACCEL keyAccelerators[] = {
		{FCONTROL | FVIRTKEY, 0x4F, ID_FILE_OPEN},
		{FCONTROL | FVIRTKEY, 0x53, ID_FILE_SAVE}
	};

	if(IsProcessRunning(lpszCmdLine))
	{
		return EXIT_SUCCESS;
	}

	OpenFile(lpszCmdLine, &g_MainWindow);

	hAccel = CreateAcceleratorTable(keyAccelerators, sizeof(keyAccelerators) / sizeof(keyAccelerators[0]));

	return app.Run(&g_MainWindow, hAccel);
}