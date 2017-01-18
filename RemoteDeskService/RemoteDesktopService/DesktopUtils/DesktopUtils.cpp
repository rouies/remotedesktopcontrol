// DesktopUtils.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <WtsApi32.h>
#include <UserEnv.h>
#include <atltypes.h>
#include "DesktopUtils.h"

static int full_screen_width;
static int full_screen_height;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		full_screen_width  = GetSystemMetrics(SM_CXSCREEN);
		full_screen_height = GetSystemMetrics(SM_CYSCREEN);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


#ifdef ENABLE_MOUSE_EVENT
EXTERN_C DESKTOPUTILS_API int MouseMove(int x, int y){
	double g_scrx = (65535.0 / full_screen_width);
	double g_scry = (65535.0 / full_screen_height);
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = (MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE);
	inp[0].mi.dx = (long)(g_scrx * x + 0.5);
	inp[0].mi.dy = (long)(g_scry * y + 0.5);
	inp[0].mi.time = 0;
	inp[0].mi.mouseData = 0;
	return SendInput(1, inp, sizeof(INPUT));;
}
EXTERN_C DESKTOPUTILS_API int LBMouseDown(){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	inp[0].mi.time = 0;
	inp[0].mi.mouseData = 0;
	return SendInput(1, inp, sizeof(INPUT));
}
EXTERN_C DESKTOPUTILS_API int LBMouseUp(){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	inp[0].mi.time = 0;
	inp[0].mi.mouseData = 0;
	return SendInput(1, inp, sizeof(INPUT));
}
EXTERN_C DESKTOPUTILS_API int RBMouseDown(){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	inp[0].mi.time = 0;
	inp[0].mi.mouseData = 0;
	return SendInput(1, inp, sizeof(INPUT));
}
EXTERN_C DESKTOPUTILS_API int RBMouseUp(){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	inp[0].mi.time = 0;
	inp[0].mi.mouseData = 0;
	return SendInput(1, inp, sizeof(INPUT));
}
#endif

#ifdef ENABLE_KEYBOARD_EVENT
EXTERN_C DESKTOPUTILS_API int KeyBoardDown(int ch){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_KEYBOARD;
	inp[0].ki.time = 0;
	inp[0].ki.wVk = ch;
	return SendInput(1, inp, sizeof(INPUT));
}
EXTERN_C DESKTOPUTILS_API int KeyBoardUp(int ch){
	INPUT inp[1];
	ZeroMemory(inp, sizeof(inp));
	inp[0].type = INPUT_KEYBOARD;
	inp[0].ki.dwFlags = KEYEVENTF_KEYUP;
	inp[0].ki.time = 0;
	inp[0].ki.wVk = ch;
	return SendInput(1, inp, sizeof(INPUT));
}
#endif

#ifdef ENABLE_SCREEN_IMAGE

EXTERN_C DESKTOPUTILS_API int GetBitImageByFullScreen(LBitMapInfo info){
	CRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = full_screen_width;
	rect.bottom = full_screen_height;
	HBITMAP hMap = CopyScreenToBitmap(rect);
	int result = SaveBitmapToMemory(hMap, info,true);
	DeleteObject(hMap);
	return result;
}

EXTERN_C DESKTOPUTILS_API int DecompressImage(LBitMapInfo info){
	ULONG inLen = info->size;
	ULONG outLen = info->src_size;
	int error = 0;
	BYTE* result = BitMapDecompress(info->out, inLen, &outLen, &error);
	if (error == BITIMAGEUTILS_NO_ERROR){
		for (ULONG i = 0; i < outLen; i++){
			info->out[i] = result[i];
		}
	}
	info->size = outLen;
	return error;
}

EXTERN_C DESKTOPUTILS_API void GetScreenSize(int* width, int* height){
	*width = full_screen_width;
	*height = full_screen_height;
}

#endif


#ifdef ENABLE_SYSTEM_EVENT

bool EnablePrivilege(LPCWSTR lpName, DWORD DesiredAccess)
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), DesiredAccess, &hToken))
	{
		return   false;
	}
	if (!LookupPrivilegeValue(NULL, lpName, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		return false;
	}
	return true;
}



EXTERN_C DESKTOPUTILS_API int LogoutWindows(bool force)
{
	UINT op = EWX_LOGOFF;
	if (force){
		op |= EWX_FORCE;
	}
	return !ExitWindowsEx(op, 0);
}


EXTERN_C DESKTOPUTILS_API int CloseWindows(bool force)
{
	if (EnablePrivilege(SE_SHUTDOWN_NAME, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY)){
		UINT op = EWX_SHUTDOWN;
		if (force){
			op |= EWX_FORCE;
		}
		ExitWindowsEx(op, 0);
		return 1;
	}
	else {
		return 0;
	}
}


__int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime;
	return   (b - a);
}

EXTERN_C DESKTOPUTILS_API int GetCpuUsageRate(int* rate)
{
	int status = 0;
	FILETIME idleTime;   //空闲时间
	FILETIME kernelTime; //内核时间
	FILETIME userTime;   //用户时间
	status = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	if (!status){
		return 1;
	}
	FILETIME preidleTime = idleTime;
	FILETIME prekernelTime = kernelTime;
	FILETIME preuserTime = userTime;
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled
	WaitForSingleObject(hEvent, 400); //等待500毫秒
	status = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	if (!status){
		return 1;
	}
	__int64 idle = CompareFileTime(preidleTime, idleTime);
	__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
	__int64 user = CompareFileTime(preuserTime, userTime);
	*rate = (kernel + user - idle) * 100 / (kernel + user);
	return status;
}

EXTERN_C DESKTOPUTILS_API int GetGlobalMemoryUsageRate(int* rate)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	if (GlobalMemoryStatusEx(&statex)){
		*rate = statex.dwMemoryLoad;
	}
	else {
		return 1;
	}
	return 0;
}

LSTATUS WriteRegValue(LPCWSTR path, LPCWSTR item, LPCWSTR value){
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		state = RegSetValueEx(hKey, item, NULL, REG_SZ, (BYTE*)value, lstrlen(value) * 2);
	}
	RegCloseKey(hKey);
	return state;
}

LSTATUS ReadRegValue(LPCWSTR path, LPCWSTR item, TCHAR result[]){
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	DWORD dwSize = 300;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		state = RegQueryValueEx(hKey, item, NULL, NULL, (LPBYTE)result, &dwSize);
	}
	RegCloseKey(hKey);
	return state;
}

EXTERN_C DESKTOPUTILS_API int SetAuthCode(LPCWSTR value)
{
	return WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Value", value);
}

EXTERN_C DESKTOPUTILS_API int SetAuthLock(LPCWSTR value)
{
	int result = WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Value", L"010101");
	if(result == ERROR_SUCCESS){
		result = WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Lock", value);
	}
	return result;
}

EXTERN_C DESKTOPUTILS_API int SetMHID(LPCWSTR value)
{
	return WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"ID", value);
}

EXTERN_C DESKTOPUTILS_API int SetLoginName(LPCWSTR value)
{
	return WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"LoginName", value);
}

EXTERN_C DESKTOPUTILS_API int SetSupperCode(LPCWSTR value)
{
	return WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"SupperCode", value);
}

EXTERN_C DESKTOPUTILS_API int CreateUserProcess(LPWSTR app)
{
	SECURITY_ATTRIBUTES sa;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = TEXT("winsta0\\default");
	DWORD sessionId = WTSGetActiveConsoleSessionId();
	if (sessionId == 0xFFFFFFFF){
		return 1;
	}
	HANDLE hToken = NULL;
	HANDLE hDupedToken = NULL;
	LPVOID lpEnvironment = NULL;
	if (!WTSQueryUserToken(sessionId, &hToken)){
		return 2;
	}
	if (!EnablePrivilege(SE_TCB_NAME, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
		| TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
		| TOKEN_READ | TOKEN_WRITE)){
		return 6;
	}
	if (!DuplicateTokenEx(hToken, GENERIC_ALL, &sa, SECURITY_IMPERSONATION_LEVEL::SecurityIdentification, TOKEN_TYPE::TokenPrimary, &hDupedToken)){
		return 3;
	}
	if (!CreateEnvironmentBlock(&lpEnvironment, hDupedToken, false))
	{
		CloseHandle(hDupedToken);
		return 4;
	}
	int result = 0;
	if (!CreateProcessAsUser(hDupedToken, NULL, app, &sa, &sa, FALSE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi)){
		result = GetLastError();
	}
	if (pi.hProcess != NULL)
	{
		CloseHandle(pi.hProcess);
	}
	if (pi.hThread != NULL)
	{
		CloseHandle(pi.hThread);
	}
	if (hDupedToken != NULL)
	{
		CloseHandle(hDupedToken);
	}
	return result;
}

EXTERN_C DESKTOPUTILS_API int ShowMessage(LPWSTR title,LPWSTR message)
{
	DWORD sessionId = WTSGetActiveConsoleSessionId();
	DWORD pResponse = 0;
	if (sessionId == 0xFFFFFFFF){
		return 1;
	}
	return !WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, sessionId, title, lstrlen(title) * 2 , message, lstrlen(message) * 2 , 0, MB_OK, &pResponse, false);
	 
}

#endif
