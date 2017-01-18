//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) 2006 Microsoft Corporation. All rights reserved.
//
// CCommandWindow provides a way to emulate external "connect" and "disconnect" 
// events, which are invoked via toggle button on a window. The window is launched
// and managed on a separate thread, which is necessary to ensure it gets pumped.
//

#pragma once

#include <windows.h>
#include "SSProvider.h"

class CCommandWindow
{
public:
    CCommandWindow(void);
    ~CCommandWindow(void);
    HRESULT Initialize();
    BOOL GetConnectedStatus();

private:
    HRESULT _MyRegisterClass(void);
    HRESULT _InitInstance();
    BOOL _ProcessNextMessage();
	BOOL GetEditText(LPTSTR lp);
    
    static DWORD WINAPI _ThreadProc(LPVOID lpParameter);
    static LRESULT CALLBACK    _WndProc(HWND, UINT, WPARAM, LPARAM);

    //CSSProvider                *_pProvider;        // Pointer to our owner.
    HWND                        _hWnd;                // Handle to our window.
    HWND                        _hWndButton;        // Handle to our window's button.
	HWND						_hWndEdit;
    HINSTANCE                    _hInst;                // Current instance
    BOOL                        _fConnected;        // Whether or not we're connected.
};
