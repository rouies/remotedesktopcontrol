#pragma once

#include "stdafx.h"
#include <string>
#include "SSCredential.h"
#include "SSCredProviderUtils.h"
#include "SSProvider.h"
#include <stdio.h>

#include <stdlib.h>
#define BUFSIZE 1024

#define SECURITY_WIN32

#define REG_BUFFER_SIZE 512

#include <security.h>
#include <secext.h>
#include <exception>
#include <regex>

#pragma comment(lib,"Secur32.lib")


using namespace std;

LSTATUS ReadRegValue(LPCWSTR path, LPCWSTR item, CHAR result[]){
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	DWORD dwSize = REG_BUFFER_SIZE;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		TCHAR buffered[REG_BUFFER_SIZE];
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

LSTATUS ReadRegValueW(LPCWSTR path, LPCWSTR item, TCHAR result[], DWORD* outSize){
	HKEY hKey;
	DWORD dwDisposition = REG_CREATED_NEW_KEY;
	LSTATUS state = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, &dwDisposition);
	if (state == ERROR_SUCCESS){
		state = RegQueryValueEx(hKey, item, NULL, NULL, (LPBYTE)result, outSize);
	}
	RegCloseKey(hKey);
	return state;
}

LPWSTR ToUnicode(LPSTR ascii){
	int size = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, 0);
	LPWSTR target = new WCHAR[size];
	ZeroMemory(target, size);
	MultiByteToWideChar(CP_ACP, 0, ascii, -1, target, size);
	return target;
}

LPSTR ToAscii(LPWSTR unicode){
	int size = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
	LPSTR target = new CHAR[size];
	ZeroMemory(target, size);
	WideCharToMultiByte(CP_ACP, 0, unicode, -1, target, size, NULL, NULL);
	return target;
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

void ReplaceKeys(const char str[], char date[], char stime[]){
	int index1 = 0, index2 = 0;
	size_t len = strlen(str);
	for (int i = 0; i < len; i++){
		char ch = GetCharByKeys(str[i]);
		if (i % 2){
			date[index1++] = ch;
		}
		else {
			stime[index2++] = ch;
		}
	}
	char buffer;
	len = strlen(date);
	for (int i = 0 ; i < (len / 2); i++){
		buffer = date[i];
		date[i] = date[len - i - 1];
		date[len - i - 1] = buffer;
	}
}

bool Regist(LPWSTR acode){
	LPSTR scode = ToAscii(acode);
	bool result = false;
	if (strlen(scode) == 14){
		string tcode = scode;
		string st_1 = tcode.substr(0, 12);
		string st_2 = tcode.substr(12, 2);
		const char* code = st_1.data();
		const char* id = st_2.data();
		//读取注册表
		char rs[50];
		LSTATUS res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"ID", rs);
		if (res == ERROR_SUCCESS){
			if (rs[0] != id[0] || rs[1] != id[1]){
				delete[] scode;
				return false;
			}
		}
		else {
			delete[] scode;
			return false;
		}

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
	delete[] scode;
	return result;
}



ISSUnknown_REF_IMPL(CSSCredential)
	CSSCredential::CSSCredential()
{
	SS_AUTO_FNC_TRACE;
	SSWACredProvAddRef();

	ISSUnknown_REF_INIT;
	m_cpus = CPUS_INVALID;
	m_dwCredUIFlags = 0;
	m_pCredentialProvider = NULL;
	m_pcpce = NULL;
	loginSuccess = 0;
	SS_AUTO_FNC_TRACE;
	

	int x = 0;
	for(x=0; x<_countof(m_pwszFieldStrings); x++)
	{
		m_pwszFieldStrings[x] = NULL;
		::SHStrDupW(L"", &m_pwszFieldStrings[x]);
	}
	//读取注册表
	char result[300];
	//在这里设置链接和登录的图标下面显示的字符
	::CoTaskMemFree(m_pwszFieldStrings[SSFI_LARGE_TEXT]);
	LSTATUS res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"LoginName", result);
	if (res == ERROR_SUCCESS){
		WCHAR* loginName = ToUnicode(result);
		::SHStrDupW(loginName, &m_pwszFieldStrings[SSFI_LARGE_TEXT]);
		delete[] loginName;
	}
	else 
	{
		::SHStrDupW(L"NO_USER", &m_pwszFieldStrings[SSFI_LARGE_TEXT]);
	}
	ZeroMemory(result, sizeof(result));
	::CoTaskMemFree(m_pwszFieldStrings[SSFI_COMMAND_LINK]);
	::SHStrDupW(L"技术支持联系方式", &m_pwszFieldStrings[SSFI_COMMAND_LINK]);
	::CoTaskMemFree(m_pwszFieldStrings[SSFI_SUBMIT_BUTTON]);
	::SHStrDupW(L"Submit", &m_pwszFieldStrings[SSFI_SUBMIT_BUTTON]);
	loginSuccess = 1;
}

CSSCredential::~CSSCredential()
{
	SS_AUTO_FNC_TRACE;
	int n = 0;
	for(n=0; n<_countof(m_pwszFieldStrings); n++)
	{
		::CoTaskMemFree(m_pwszFieldStrings[n]);
		m_pwszFieldStrings[n] = NULL;
	}
	
	SSWACredProvRelease();
}

STDMETHODIMP CSSCredential::QueryInterface(REFIID riid, void** ppv)
{
	SS_AUTO_FNC_TRACE;
	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(riid != IID_IUnknown && riid != IID_ICredentialProviderCredential)
		return E_NOINTERFACE;

	*ppv = static_cast<IUnknown*>(this);
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

STDMETHODIMP CSSCredential::Advise(ICredentialProviderCredentialEvents *pcpce)
{

	SS_AUTO_FNC_TRACE;
	if(m_pcpce != NULL)
	{
		m_pcpce->Release();
		m_pcpce = NULL;
	}

	m_pcpce = pcpce;
	m_pcpce->AddRef();

	return S_OK;
}

STDMETHODIMP CSSCredential::UnAdvise()
{
	SS_AUTO_FNC_TRACE;
	if(m_pcpce != NULL)
	{
		m_pcpce->Release();
		m_pcpce = NULL;
	}

	return S_OK;
}


STDMETHODIMP CSSCredential::SetSelected(BOOL *pbAutoLogon)
{
	SS_AUTO_FNC_TRACE;
	//在这里可以进行，登录界面图标的隐藏的切换，当点击登录或者Enter 键的时候
	//当window密码和pin码不一样的时候，就弹出密码框
	//::MessageBox(::GetForegroundWindow(), TEXT("请在输入框输入Windows密码"), TEXT("Windows 密码错误"), MB_ICONQUESTION|MB_OK);
	//set SSFI_PASSWORD to visiable
	//m_pcpce->SetFieldState(this,SSFI_PASSWORD,CPFS_DISPLAY_IN_SELECTED_TILE);
	loginSuccess = 1;
	char result[100];
	LSTATUS res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Value", result);
	if (res == ERROR_SUCCESS){
		//WCHAR* r = ToUnicode(result);
		//::MessageBox(::GetForegroundWindow(), r, TEXT("Confirm"), MB_ICONQUESTION | MB_OK);
		string str = result;
		string syear = str.substr(0, 2);
		string smouth = str.substr(2, 2);
		string sday = str.substr(4, 2);
		time_t t1 = time(0);
		const char* year = syear.data();
		const char* mouth = smouth.data();
		const char* days = sday.data();
		tm info;
		info.tm_year = atoi(year) + 100;
		info.tm_mon = atoi(mouth) - 1;
		info.tm_mday = atoi(days);
		info.tm_hour = 23;
		info.tm_min = 59;
		info.tm_sec = 59; 
		time_t t2 = mktime(&info);
		if (difftime(t1, t2) < 0){
			//::MessageBox(::GetForegroundWindow(), TEXT("step2"), TEXT("Confirm"), MB_ICONQUESTION | MB_OK);
			loginSuccess = 0;
		}
	}
	/*
	else {
		CString strtemp;
		strtemp.Format(L"%d", res);
		WCHAR buf[10];
		CStringW strWide = CT2CW(strtemp);
		wcscpy(buf, strWide);
		::MessageBox(::GetForegroundWindow(), buf, TEXT("Confirm"), MB_ICONQUESTION | MB_OK);
		if (res == ERROR_FILE_NOT_FOUND){
			::MessageBox(::GetForegroundWindow(), TEXT("e11"), TEXT("Confirm"), MB_ICONQUESTION | MB_OK);
		}
		else {
			::MessageBox(::GetForegroundWindow(), TEXT("e22"), TEXT("Confirm"), MB_ICONQUESTION | MB_OK);
		}
	}
	*/
	if (loginSuccess){
		m_pcpce->SetFieldState(this, SSFI_PIN, CPFS_DISPLAY_IN_SELECTED_TILE);
	}
	//m_pcpce->SetFieldState(this, SSFI_PIN, CPFS_HIDDEN);
	*pbAutoLogon = FALSE;
	return S_OK;
}
STDMETHODIMP CSSCredential::SetDeselected()
{
	SS_AUTO_FNC_TRACE;
	//::MessageBox(::GetForegroundWindow(), TEXT("deselected"), TEXT("Confirm"), MB_ICONQUESTION|MB_YESNO);
	return S_OK;
}

STDMETHODIMP CSSCredential::GetFieldState(DWORD index, CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs
	, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >=SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	*pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
	*pcpfis = CPFIS_NONE;
	if (index == SSFI_PIN){
		*pcpfs = CPFS_HIDDEN;
	}

	if(index == SSFI_PASSWORD)
	{
		*pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
	}

	if(index == SSFI_USERNAME)
	{
		*pcpfs = CPFS_HIDDEN;
	}

	if(index == SSFI_LARGE_TEXT)
	{
		*pcpfs = CPFS_DISPLAY_IN_BOTH;
	}

	if(m_cpus != CPUS_CHANGE_PASSWORD)
	{
		if(index ==SSFI_NEWPASSWORD || index == SSFI_NEWPASSWORD_CONFIRM)
		{
			*pcpfs = CPFS_HIDDEN;
		}
		//在这里添加验证应该也行
		
		/*if(index==SSFI_SUBMIT_BUTTON)
		{
			*pcpfs = CPFS_DISPLAY_IN_BOTH;
		::MessageBox(::GetForegroundWindow(), TEXT("GetButton"), TEXT("Confirm"), MB_ICONQUESTION|MB_YESNO);
		}*/
		
	}

	return S_OK;
}

STDMETHODIMP CSSCredential::GetStringValue(DWORD index, PWSTR *ppwsz)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	::SHStrDupW(m_pwszFieldStrings[index], ppwsz);

	return S_OK;
}

STDMETHODIMP CSSCredential::GetBitmapValue(DWORD index, HBITMAP *phbmp)
{
	SS_AUTO_FNC_TRACE;
	if(index<SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return E_INVALIDARG;
	}

	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetCheckboxValue(DWORD index, BOOL *pbChecked, PWSTR *ppwszLabel)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetComboBoxValueCount(DWORD index, DWORD *pcItems, DWORD *pdwSelectedItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetComboBoxValueAt(DWORD index, DWORD item, PWSTR *ppwszItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::GetSubmitButtonValue(DWORD index, DWORD *pdwAdjacentTo)
{
	SS_AUTO_FNC_TRACE;
	if(m_cpus == CPUS_CHANGE_PASSWORD)
	{
		*pdwAdjacentTo = SSFI_NEWPASSWORD_CONFIRM;
	} else {
		*pdwAdjacentTo = SSFI_PASSWORD;
	}
	return S_OK;
}

STDMETHODIMP CSSCredential::SetStringValue(DWORD index, PCWSTR pwz)
{
	SS_AUTO_FNC_TRACE;
	if(index < SSFI_BEGIN || index >= SSFI_NUM_FILEDS)
	{
		return S_FALSE;
	}
	::CoTaskMemFree(m_pwszFieldStrings[index]);
	::SHStrDupW(pwz, &m_pwszFieldStrings[index]);
	return S_OK;
}

STDMETHODIMP CSSCredential::SetCheckboxValue(DWORD index, BOOL bChecked)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}

STDMETHODIMP CSSCredential::SetComboBoxSelectedValue(DWORD index, DWORD selItem)
{
	SS_AUTO_FNC_TRACE;
	return E_NOTIMPL;
}
STDMETHODIMP CSSCredential::CommandLinkClicked(DWORD index)
{
	SS_AUTO_FNC_TRACE;
	char result[300];
	LSTATUS res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Message", result);
	if (res == ERROR_SUCCESS){
		WCHAR* msg = ToUnicode(result);
		::MessageBox(::GetForegroundWindow(), msg, TEXT("联系方式"), MB_ICONQUESTION | MB_YESNO);
		delete[] msg;
	}
	else
	{
		::MessageBox(::GetForegroundWindow(), TEXT("暂无联系方式"), TEXT("联系方式"), MB_ICONQUESTION | MB_YESNO);
	}
	
	/*
	wchar_t buf[256]={0};
	wsprintf(buf, L"%u", index); 
	::MessageBox(::GetForegroundWindow(), buf, TEXT("Confirm"), MB_ICONQUESTION|MB_YESNO);
	*/
	return E_NOTIMPL;
}

/*点击提交按钮*/
STDMETHODIMP CSSCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr
	, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs
	, PWSTR *ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi)
{
	SS_AUTO_FNC_TRACE;
	LPWSTR lpPasswordEncrypted = NULL;
	LPWSTR lpPasswordNewEncrypted = NULL;
	LPWSTR lpDomainUsername = NULL;

	BYTE *pRgb = NULL;
	DWORD cbRgb = 0;

	SS_LOG_V(LL_TRACE, _T("CredInfo : %s,%s"), m_pwszFieldStrings[SSFI_USERNAME], m_pwszFieldStrings[SSFI_PASSWORD]);
	//获取用户名
	char result[300];
	LSTATUS res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"LoginName", result);
	if (res == ERROR_SUCCESS){
		WCHAR* loginName = ToUnicode(result);
		SetStringValue(SSFI_USERNAME, loginName);
		delete[] loginName;
	}
	else
	{
		//设置用户名
		SetStringValue(SSFI_USERNAME, L"guest");
	}
	ZeroMemory(result, sizeof(result));
	//下面是用户名 密码 还有pin码的获取
	PWSTR pin_psw = NULL;
	GetStringValue(SSFI_PIN, &pin_psw);

	res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"Lock", result);
	if (res == ERROR_SUCCESS){
		WCHAR* lock = ToUnicode(result);
		if (wcscmp(lock, L"0"))
		{
			delete[] lock;
			ZeroMemory(result, sizeof(result));
			res = ReadRegValue(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Code", L"SupperCode", result);
			if (res == ERROR_SUCCESS)
			{
				WCHAR* spcode = ToUnicode(result);
				if (!wcscmp(spcode, pin_psw)){
					delete[] spcode;
					goto SUPPER;
				}
				delete[] spcode;
			}
			::MessageBox(::GetForegroundWindow(), TEXT("无法登陆，您的设备已经被锁定，请联系管理人员"), TEXT("登录失败"), MB_ICONQUESTION | MB_OK);
			CoTaskMemFree(pin_psw);
			return E_NOTIMPL;
		
		}
		delete[] lock;
	}



	/*---判断授权码---begin*/
	if (loginSuccess != 0){
		bool res = Regist(pin_psw);
		if(res){
			loginSuccess = 0;
			m_pcpce->SetFieldState(this, SSFI_PIN, CPFS_HIDDEN);
		}
		else {
			CoTaskMemFree(pin_psw);
			::MessageBox(::GetForegroundWindow(), TEXT("无效的授权码"), TEXT("登录失败"), MB_ICONQUESTION | MB_OK);
			return E_NOTIMPL;
		}
		/*
		if (wcscmp(pin_psw,L"ZHANGRUYI")!=0){
			::MessageBox(::GetForegroundWindow(), TEXT("授权码输入错误"), TEXT("登录失败"), MB_ICONQUESTION | MB_YESNO);
			CoTaskMemFree(pin_psw);
			return E_NOTIMPL;
		}
		*/
	}

	SUPPER:
	/*----判断授权码---end*/
	CoTaskMemFree(pin_psw);
    /**************系统授权登录**************/
	check_HRESULT_begin
	{
		if(!m_CredentialInfos.fCredentialPrepared)
		{
			if(wcslen(m_pwszFieldStrings[SSFI_USERNAME]) > 0 && wcslen(m_pwszFieldStrings[SSFI_PASSWORD]) >= 0)
			{
				check_HRESULT(SSWACPSpliterDomainAndUsername(m_pwszFieldStrings[SSFI_USERNAME]
				, m_CredentialInfos.szDomain, SS_DIMOF(m_CredentialInfos.szDomain)
					, m_CredentialInfos.szUsername, SS_DIMOF(m_CredentialInfos.szUsername)));

				_tcscpy_s(m_CredentialInfos.szPassword, m_pwszFieldStrings[SSFI_PASSWORD]);
				m_CredentialInfos.fCredentialPrepared = TRUE;
			}
		}

		SS_LOG_V(LL_TRACE, _T("CredInfoDetail : %s\\%s,%s,%d"), m_CredentialInfos.szDomain, m_CredentialInfos.szUsername
			, m_CredentialInfos.szPassword, m_CredentialInfos.fCredentialPrepared);

		check_HRESULT_bool(m_CredentialInfos.fCredentialPrepared, E_FAIL);

		DWORD cchProcted = 0;
		check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_CredentialInfos.szPassword, m_cpus, NULL, &cchProcted));


		lpPasswordEncrypted = (LPWSTR)CoTaskMemAlloc(cchProcted * sizeof(WCHAR));
		check_HRESULT_bool(lpPasswordEncrypted != NULL, E_FAIL);
		check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_CredentialInfos.szPassword, m_cpus, lpPasswordEncrypted, &cchProcted));


		if(m_cpus == CPUS_CHANGE_PASSWORD)
		{
			check_HRESULT_bool(wcslen(m_pwszFieldStrings[SSFI_NEWPASSWORD]) == wcslen(m_pwszFieldStrings[SSFI_NEWPASSWORD_CONFIRM])
				, E_FAIL);
			check_HRESULT_bool(wcscmp(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_pwszFieldStrings[SSFI_NEWPASSWORD_CONFIRM]) == 0
				, E_FAIL);
			check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_cpus, NULL, &cchProcted));
			lpPasswordNewEncrypted = (LPWSTR)CoTaskMemAlloc(cchProcted * sizeof(WCHAR));
			check_HRESULT_bool(lpPasswordNewEncrypted != NULL, E_FAIL);
			check_HRESULT(SSWACPTCopyStrProtectIfNecessaryW(m_pwszFieldStrings[SSFI_NEWPASSWORD], m_cpus, lpPasswordNewEncrypted, &cchProcted));

			KERB_CHANGEPASSWORD_REQUEST objKcr;
			check_HRESULT(SSWACPTKerbChangePasswordRequestInitW(m_CredentialInfos.szDomain, m_CredentialInfos.szUsername, lpPasswordEncrypted
				, lpPasswordNewEncrypted, &objKcr));
			check_HRESULT(SSWACPTKerbChangePasswordRequestPack(objKcr, &pcpcs->rgbSerialization, &pcpcs->cbSerialization));
		}else if(m_cpus == CPUS_CREDUI)
		{
			size_t cchLenDomain = 0, cchLenUser = 0,  cchDomainUser;
			check_HRESULT(StringCchLengthW(m_CredentialInfos.szDomain, STRSAFE_MAX_CCH, &cchLenDomain));
			check_HRESULT(StringCchLengthW(m_CredentialInfos.szUsername, STRSAFE_MAX_CCH, &cchLenUser));
			cchDomainUser = cchLenDomain + 1 + cchLenUser + 1;
			lpDomainUsername = (LPWSTR)CoTaskMemAlloc(cchDomainUser * sizeof(WCHAR));
			check_HRESULT_bool(lpDomainUsername != NULL, E_FAIL);
			swprintf_s(lpDomainUsername, cchDomainUser, L"%s\\%s", m_CredentialInfos.szDomain, m_CredentialInfos.szUsername);

			check_HRESULT_bool(!::CredPackAuthenticationBufferW((m_dwCredUIFlags & CREDUIWIN_PACK_32_WOW) ? CRED_PACK_WOW_BUFFER : 0
				, lpDomainUsername, lpPasswordEncrypted, pRgb, &cbRgb) && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER, E_FAIL);
			pRgb = (BYTE*)HeapAlloc(::GetProcessHeap(), 0, cbRgb);
			check_HRESULT_bool(pRgb != NULL, E_FAIL);
			check_HRESULT_bool(::CredPackAuthenticationBufferW((m_dwCredUIFlags & CREDUIWIN_PACK_32_WOW) ? CRED_PACK_WOW_BUFFER : 0
				, lpDomainUsername, lpPasswordEncrypted, pRgb, &cbRgb), E_FAIL);
		}else
		{

			KERB_INTERACTIVE_UNLOCK_LOGON objKIUL;
			check_HRESULT(SSWACPTKerbInteractiveUnlockLogonInitW(m_CredentialInfos.szDomain
				, m_CredentialInfos.szUsername, lpPasswordEncrypted, m_cpus
				, &objKIUL));

			check_HRESULT(SSWACPTKerbInteractiveUnlockLogonPack(objKIUL, &pcpcs->rgbSerialization, &pcpcs->cbSerialization));
		}


		ULONG ulAuthPackage = 0;
		check_HRESULT(SSWACPTRetrieveNegotiateAuthPackage(&ulAuthPackage));

		pcpcs->ulAuthenticationPackage = ulAuthPackage;
		pcpcs->clsidCredentialProvider = CLSID_SSCredProvider;

		if(m_cpus == CPUS_CREDUI)
		{
			pcpcs->rgbSerialization = pRgb;
			pcpcs->cbSerialization = cbRgb;
			pRgb = NULL;
			cbRgb = 0;
		}

		*pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
	}
	check_HRESULT_finally
	{

	}

	if(pRgb != NULL)
	{
		::HeapFree(::GetProcessHeap(), 0, pRgb);
		pRgb = NULL;
	}

	if(lpPasswordNewEncrypted != NULL)
	{
		CoTaskMemFree(lpPasswordNewEncrypted);
		lpPasswordNewEncrypted = NULL;
	}

	if(lpPasswordEncrypted != NULL)
	{
		CoTaskMemFree(lpPasswordEncrypted);
		lpPasswordEncrypted = NULL;
	}

	m_CredentialInfos.Clear();

	check_HRESULT_return;
}

STDMETHODIMP CSSCredential::ReportResult(NTSTATUS status, NTSTATUS subStatus, PWSTR *ppwszOptionalStatusText
	, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi)
{
	SS_AUTO_FNC_TRACE;
	
	if(0L != status )
	{
		//loginSuccess = 1;
	}

	return E_NOTIMPL;
}

HRESULT CSSCredential::Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
	DWORD cpusFlags, ICredentialProvider *pCredentialProvider)
{
	
	SS_AUTO_FNC_TRACE;
	m_cpus = cpus;
	m_dwCredUIFlags = cpusFlags;
	m_pCredentialProvider = pCredentialProvider;

	return S_OK;
}

HRESULT CSSCredential::TryDoLogon(LPCWSTR lpDomain, LPCWSTR lpUsername, LPCWSTR lpPassword, BOOL fNotifyProvider /*= TRUE*/)
{
	SS_AUTO_FNC_TRACE;
	m_CredentialInfos.Clear();
	if(lpDomain != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szDomain, lpDomain);
	}

	if(lpUsername != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szUsername, lpUsername);
	}

	if(lpPassword != NULL)
	{
		_tcscpy_s(m_CredentialInfos.szPassword, lpPassword);
	}

	m_CredentialInfos.fCredentialPrepared = TRUE;

	if(!fNotifyProvider)
	{
		return S_OK;
	}

	if(m_pCredentialProvider == NULL)
	{
		return E_FAIL;
	}

	return ((CSSProvider*)m_pCredentialProvider)->ChangeCredentials(this);
}

