// RegEdit.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

void parse(){
	LPCWSTR path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code";
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	DWORD dwSize;
	long state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		LPCWSTR result = new TCHAR[80];
		state = RegQueryValueEx(hKey, L"Value", NULL, NULL, (LPBYTE)result, &dwSize);
		LPCWSTR val = L"1234567890";
		if (state == ERROR_FILE_NOT_FOUND){
			state = RegSetValueEx(hKey, L"Value", NULL, REG_SZ, (BYTE*)val, lstrlen(val) * 2);
			cout << "error:" << state;
		}
		else if (state == ERROR_SUCCESS){
			int size = WideCharToMultiByte(CP_ACP, 0, result, dwSize, NULL, 0, NULL, NULL);
			LPSTR target = new CHAR[size];
			ZeroMemory(target, size);
			WideCharToMultiByte(CP_ACP, 0, result, dwSize, target, size, NULL, NULL);
			cout << "success:" << target;
			delete[] target;
		}
		delete[] result;
	}
	else {
		cout << "error1";
	}
	RegCloseKey(hKey);
}

LPWSTR ToUnicode(LPSTR ascii){
	int size = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, 0);
	LPWSTR target = new WCHAR[size];
	ZeroMemory(target, size);
	MultiByteToWideChar(CP_ACP, 0, ascii,-1, target, size);
	return target;
}

LPSTR ToAscii(LPWSTR unicode){
	int size = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
	LPSTR target = new CHAR[size];
	ZeroMemory(target, size);
	WideCharToMultiByte(CP_ACP, 0, unicode, -1, target, size, NULL, NULL);
	return target;
}


LSTATUS ReadRegValue(LPCWSTR path, LPCWSTR item, CHAR result[]){
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	DWORD dwSize;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		TCHAR buffered[40];
		state = RegQueryValueEx(hKey, item, NULL, NULL, (LPBYTE)buffered, &dwSize);
		if (state == ERROR_SUCCESS){
			int size = WideCharToMultiByte(CP_ACP, 0, buffered, dwSize, NULL, 0, NULL, NULL);
			LPSTR target = new CHAR[size];
			ZeroMemory(target, size);
			WideCharToMultiByte(CP_ACP, 0, buffered, dwSize, target, size, NULL, NULL);
			strcpy_s(result, size, target);
			delete[] target;
		}
	}
	RegCloseKey(hKey);
	return state;
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

char keys[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

char values[] = {
	'F', 'g', 'a', 'I', 's', 'm', 'L', 'M', 'n', 'O'
};

char GetCharByKeys(char ch){
	for (int i = 0, len = sizeof(values); i < len; i++){
		if (values[i] == ch){
			return keys[i];
		}
	}
	return '0';
}

void ReplaceKeys(char str[],char date[],char stime[]){
	int index1 = 0, index2 = 0;
	for (int i = 0, len = strlen(str); i < len; i++){
		char ch = GetCharByKeys(str[i]);
		if (i % 2){
			date[index1++] = ch;
		}
		else {
			stime[index2++] = ch;
		}
	}
	char buffer;
	for (int i = 0, len = strlen(date); i < (len / 2); i++){
		buffer = date[i];
		date[i] = date[len - i - 1];
		date[len - i - 1] = buffer;
	}
}

bool Regist(LPWSTR acode){
	LPSTR code = ToAscii(acode);
	bool result = false;
	if (strlen(code) == 12){
		char date[7], stime[7];
		date[6] = '\0';
		stime[6] = '\0';
		ReplaceKeys(code, date, stime);
		time_t t1 = time(0);
		tm info;
		localtime_s(&info, &t1);
		char curDateStr[10];
		strftime(curDateStr, 10, "%y%m%d", &info);
		if (!strcmp(date, curDateStr)){
			int day;
			//合法注册
			if ((day = atoi(stime)) > 0){
				time_t t2 = time(0);
				t2 += day * 24 * 60 * 60;
				localtime_s(&info, &t2);
				strftime(curDateStr, 10, "%y%m%d", &info);
				LPWSTR ures = ToUnicode(curDateStr);
				result = !WriteRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Value", ures);
				delete[] ures;
			}
		}
	}
	delete[] code;
	return result;
}

LSTATUS ReadRegValueW(LPCWSTR path, LPCWSTR item, TCHAR result[], DWORD* outSize){
	HKEY hKey;
	DWORD dwSize;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		TCHAR buffer[300];
		state = RegQueryValueEx(hKey, item, NULL, NULL, (LPBYTE)buffer, &dwSize);
		wcscpy_s(result, dwSize, buffer);
	}
	RegCloseKey(hKey);
	return state;
}




int _tmain(int argc, _TCHAR* argv[])
{
	char buffer[10];
	long val = 100;
	int len = 0;
	int err = _ltoa_s(val, buffer, 10);
	cout << buffer;
	int a;
	cin >> a;
}

