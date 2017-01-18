// SSCredProvider.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SSFactory.h"

static LONG g_cRef = 0;

VOID SSWACredProvAddRef()
{
	InterlockedIncrement(&g_cRef);
}

VOID SSWACredProvRelease()
{
	InterlockedDecrement(&g_cRef);
}

STDAPI DllCanUnloadNow()
{
	return g_cRef > 0 ? S_FALSE : S_OK;
}

VOID LogPrintCB(UINT nLevel, LPCTSTR lpInfo)
{
	OutputDebugString(lpInfo);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID FAR* ppv)
{
	SS_LOG_R_SetCB(LogPrintCB);
	return CSSFactory_CreateInstance(rclsid, riid, ppv);
}
