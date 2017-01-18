// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <string>
#include <afxcontrolbars.h>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
// TODO: reference additional headers your program requires here
#include "SSCredProvider.h"
#include<fstream>
#include <iostream>
#include <tlhelp32.h>


#define ISSUnknown_REF_DEFINE \
protected: \
	ULONG m_ulRef; \
public: \
	STDMETHOD_(ULONG, AddRef)(); \
	STDMETHOD_(ULONG, Release)(); \
	STDMETHOD(QueryInterface)(CONST IID& iid, void **ppv); \
private:

#define ISSUnknown_REF_INIT \
	m_ulRef = 1

#define ISSUnknown_REF_IMPL(class) \
	STDMETHODIMP_(ULONG) class::AddRef() \
	{ \
	return ++m_ulRef; \
	} \
	STDMETHODIMP_(ULONG) class::Release() \
	{ \
	ULONG rv = --m_ulRef; \
	if(rv <= 0) \
		{ \
		rv = 0; \
		delete this; \
		} \
		return rv; \
	}

#define SSWA_MAX_DOMAIN 25
#define SSWA_MAX_USERNAME 25
#define SSWA_MAX_PASSWORD 25

typedef struct tagSSWACredProvCredentialInfos
{
	BOOL fCredentialPrepared;

	WCHAR szDomain[SSWA_MAX_DOMAIN];
	WCHAR szUsername[SSWA_MAX_USERNAME];
	WCHAR szPassword[SSWA_MAX_PASSWORD];


	tagSSWACredProvCredentialInfos()
	{
		Clear();
	}

	VOID Clear()
	{
		fCredentialPrepared = FALSE;

		_tcscpy_s(szDomain,TEXT(""));
		_tcscpy_s(szDomain, TEXT(""));
		_tcscpy_s(szDomain, TEXT(""));
	}

} SSWACredProvCredentialInfos;


#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Credui.lib")


#define SS_AUTO_FNC_TRACE
#define SS_LOG_V(...)
#define LL_TRACE
#define SS_DIMOF _countof
#define SS_LOG_R_SetCB(...)
#define SS_LOG_V_W(...)

#define MP_ASSERT_HERE DebugBreak()
/************************************************************************/
/* CHECK REGION Macros                                                  */
/************************************************************************/
#define SS_CHECK_BLOCK_AFTER_TAGFINISH 0x12345678

#define check_block_begin(r_type, r_v_e) r_type _ret_01_tmp = (r_v_e), _ret_01 = (r_v_e); int __disable_loop_tt = 0; 
#define check_block_finally(r_v_s) _ret_01 = (r_v_s); _tag_finish: __disable_loop_tt = SS_CHECK_BLOCK_AFTER_TAGFINISH;
#define check_block_return return _ret_01
#define check_block_return_is_success(r_v_s) (_ret_01 == (r_v_s))

#define check_block_exit(r_v_) {if(__disable_loop_tt != 0) MP_ASSERT_HERE; _ret_01 = (r_v_); goto _tag_finish;}
#define check_value(exp, r_v_s) {_ret_01_tmp = (exp); if(_ret_01_tmp != (r_v_s)) check_block_exit(_ret_01_tmp); }
#define check_value_noerr(exp, r_v_s) {if((exp) != (r_v_s)) check_block_exit(r_v_s); }

#define check_value_r_map(exp_type, exp, exp_v_s, exp_map) \
{ \
	exp_type _exp_01_tmp = (exp); \
	if(_exp_01_tmp != (exp_v_s))  \
	check_block_exit(exp_map(_exp_01_tmp)); \
}

//////////////////////////////////////////////////////////////////////////
#define check_HRESULT_begin check_block_begin(HRESULT, E_FAIL)
#define check_HRESULT_finally check_block_finally(S_OK)
#define check_HRESULT_return check_block_return
#define check_HRESULT_return_is_success check_block_return_is_success(S_OK)

#define check_HRESULT_exit check_block_exit
#define check_HRESULT_bool(exp, r_v) if(!(exp)) check_block_exit(r_v)
#define check_HRESULT(exp) check_value(exp, S_OK)
#define check_HRESULT_SUCCEEDED(exp) {_ret_01_tmp = (exp); check_HRESULT_bool(SUCCEEDED(_ret_01_tmp), _ret_01_tmp);}
//检测客户端是否运行
bool isInterfaceRun();
//传递PIN码
bool ShareMemory(char * pin);

//配置文件为txt的处理函数
//获取本地Ukey信息
//bool GetUkeyInfo(char * username,char * serial,char * ou);
//设置本地Ukey信息
//bool SetUkeyInfo(char * username,char * serial,char * ou);
//获取本地Ukey锁定信息
//bool GetLockInfo(char * num,char * time,char *lockTime);
//设置本地Ukey锁定信息
//bool SetLockInfo(char * num,char * time,char *lockTime);
//关机
bool ShutdownComputer();
//获取本地时间
__int64 GetNowTime();
//long类型转换为字符串类型
int LongToTime(__int64 longTime,char* charTime);
//重命名证书
int RenameCerFile(const char* fileName,const char* newFileName);
//比较Ukey历史
int CompareUkeyHostory(const char* serialNo);
//设置Ukey历史记录
//int SetUkeyHostory(const char* serialNo);

//配置文件为ini的处理函数

//窄字符转款字符
std::wstring AnsiToUnicode(const char* str_src);
//宽字符转窄字符
std::string toNarrowString( const wchar_t* pStr);
//设置ini文件信息
int  SetIniInfo(const char* appName,const char* keyName,const char* keyValue,const char* iniFileName);
//获取ini文件信息
std::string  GetIniInfo(const char* appName,const char* keyName,const char* iniFileName);
//设置Ukey信息
int SetUkeyInfo(char * username,char * serial,char * ou,const char* filePath);
//获取Ukey信息
int GetUkeyInfo(char * username,char * serial,char * ou,const char* filePath);

//设置Ukey历史记录
int SetUkeyHostory(const char* serialNo,char* num,const char* filePath);
//获取Ukey历史记录
int GetUkeyHostoryNum(char* num,const char* filePath);

//获取Ukey历史个数
int GetUkeyHostoryNum(char* num,const char* filePath);
//获取Ukey历史序列号
int GetUkeyHostorySerial(char* serialNo,char* num,const char* filePath);
//设置锁定信息
int SetLockInfo(const char *num,const char *hours,const char *lockTime,const char* filePath);
//获取锁定信息
int GetLockInfo(char *num,char *hours,char *lockTime,const char* filePath);
