#pragma once
#include <windows.h>
#include <wincred.h>
#pragma comment(lib, "Credui.lib")

int CALLBACK WinMain (
		 __in HINSTANCE hInstance,
		 __in_opt HINSTANCE hPrevInstance,
		 __in LPSTR lpCmdLine,
		 __in int nShowCmd
		 )
{
	BOOL save = false;
	DWORD authPackage = 0;
	LPVOID authBuffer;
	ULONG authBufferSize = 0;
	CREDUI_INFO credUiInfo;

	credUiInfo.pszCaptionText = TEXT("My caption");
	credUiInfo.pszMessageText = TEXT("My message");
	credUiInfo.cbSize = sizeof(credUiInfo);
	credUiInfo.hbmBanner = NULL;
	credUiInfo.hwndParent = NULL;

	CredUIPromptForWindowsCredentials(&(credUiInfo), 0, &(authPackage), 
		NULL, 0, &authBuffer, &authBufferSize, &(save), 0);

	return 0;
}
