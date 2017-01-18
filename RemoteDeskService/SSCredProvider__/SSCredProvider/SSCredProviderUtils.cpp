#include "stdafx.h"
#include "SSCredProviderUtils.h"

#include <intsafe.h>

#define SECURITY_WIN32
#include <security.h>

#pragma comment(lib, "Secur32.lib")

HRESULT SSWACPTCopyStrProtect(LPWSTR lpStr, LPWSTR lpProtectedStr, DWORD *pCchProtectedStr)
{
	check_HRESULT_begin
	{
		check_HRESULT_bool(lpStr != NULL && (lpProtectedStr != NULL || pCchProtectedStr != NULL), E_INVALIDARG);

		DWORD cchProtected = 0;
		check_HRESULT_bool(!::CredProtectW(FALSE, lpStr, (DWORD)wcslen(lpStr) + 1, NULL, &cchProtected, NULL)
			&& ::GetLastError() == ERROR_INSUFFICIENT_BUFFER && cchProtected > 0, E_FAIL);

		if(lpProtectedStr == NULL)
		{
			*pCchProtectedStr = cchProtected;
			check_HRESULT_exit(S_OK);
		}

		if(pCchProtectedStr != NULL && *pCchProtectedStr < cchProtected)
		{
			*pCchProtectedStr = cchProtected;
			check_HRESULT_exit(ERROR_INSUFFICIENT_BUFFER);
		}

		check_HRESULT_bool(::CredProtectW(FALSE, lpStr, (DWORD)wcslen(lpStr) + 1, lpProtectedStr, &cchProtected, NULL)
			, E_FAIL);

		if(pCchProtectedStr != NULL)
		{
			*pCchProtectedStr = cchProtected;
		}

	}
	check_HRESULT_finally
	{

	}

	check_HRESULT_return;
}

HRESULT SSWACPTCopyStrProtectIfNecessaryW(LPWSTR lpStr, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus,
										  LPWSTR lpProtectedStr, DWORD *pCchProtectedStr)
{
	check_HRESULT_begin
	{
		check_HRESULT_bool(lpStr != NULL && (lpProtectedStr != NULL || pCchProtectedStr != NULL)
			,E_INVALIDARG);

		BOOL fCredAlreadyEncrypted = FALSE;
		CRED_PROTECTION_TYPE tpCredProtected = CredUnprotected;

		if(::CredIsProtectedW(lpStr, &tpCredProtected))
		{
			if(tpCredProtected != CredUnprotected)
			{
				fCredAlreadyEncrypted = TRUE;
			}
		}

		if(fCredAlreadyEncrypted || cpus == CPUS_CREDUI)
		{
			if(lpProtectedStr == NULL)
			{
				*pCchProtectedStr = (DWORD)wcslen(lpStr) + 1;
				check_HRESULT_exit(S_OK);
			}

			if(pCchProtectedStr != NULL && *pCchProtectedStr < wcslen(lpStr) + 1)
			{
				*pCchProtectedStr = (DWORD)wcslen(lpStr) + 1;
				check_HRESULT_exit(ERROR_INSUFFICIENT_BUFFER);
			}

			size_t sz = (pCchProtectedStr == NULL) ? (wcslen(lpStr) + 1) : *pCchProtectedStr;
			wcscpy_s(lpProtectedStr, sz, lpStr);
		}else
		{
			check_HRESULT(SSWACPTCopyStrProtect(lpStr, lpProtectedStr, pCchProtectedStr));
		}
	}
	check_HRESULT_finally
	{

	}

	check_HRESULT_return;
}

HRESULT SSWACPTUnicodeStringInitWithStringW(LPWSTR lpStr, UNICODE_STRING *lpUniStr)
{
	check_HRESULT_begin
	{
		size_t lenStr = 0;
		check_HRESULT(::StringCchLengthW(lpStr, USHORT_MAX, &lenStr));

		USHORT usCharCount = 0;
		check_HRESULT(::SizeTToUShort(lenStr, &usCharCount));
		USHORT usCharSize = 0;
		check_HRESULT(::SizeTToUShort(sizeof(WCHAR), &usCharSize));
		check_HRESULT(::UShortMult(usCharCount, usCharSize, &(lpUniStr->Length)));
		lpUniStr->MaximumLength = lpUniStr->Length;
		lpUniStr->Buffer = lpStr;
	}
	check_HRESULT_finally
	{

	}

	check_HRESULT_return;
}

HRESULT SSWACPTKerbInteractiveUnlockLogonInitW(LPWSTR lpDomain, LPWSTR lpUsername, LPWSTR lpPassword, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus
											   , KERB_INTERACTIVE_UNLOCK_LOGON *pObjKiul)
{
	check_HRESULT_begin
	{
		KERB_INTERACTIVE_UNLOCK_LOGON objKiul;
		ZeroMemory(&objKiul, sizeof(objKiul));
		KERB_INTERACTIVE_LOGON *pObjKil = &objKiul.Logon;
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpDomain, &pObjKil->LogonDomainName));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpUsername, &pObjKil->UserName));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpPassword, &pObjKil->Password));
		switch (cpus)
		{
		case CPUS_UNLOCK_WORKSTATION:
			pObjKil->MessageType = KerbWorkstationUnlockLogon;
			break;
		case CPUS_LOGON:
			pObjKil->MessageType = KerbInteractiveLogon;
			break;
		case CPUS_CREDUI:
			pObjKil->MessageType = (KERB_LOGON_SUBMIT_TYPE)0; // MessageType does not apply to CredUI
			break;
		default:
			check_HRESULT_exit(E_INVALIDARG);
			break;
		}

		CopyMemory(pObjKiul, &objKiul, sizeof(objKiul));
	}
	check_HRESULT_finally
	{

	}

	check_HRESULT_return;
}

VOID SSWACPTUnicodeStringInitWithUnicodeStringCopy(CONST UNICODE_STRING& rus, PWSTR lpBuffer, UNICODE_STRING *pus)
{
	pus->Length = rus.Length;
	pus->MaximumLength = rus.MaximumLength;
	pus->Buffer = lpBuffer;
	CopyMemory(pus->Buffer, rus.Buffer, pus->Length);
}

HRESULT SSWACPTKerbInteractiveUnlockLogonPack(CONST KERB_INTERACTIVE_UNLOCK_LOGON &objKiul, BYTE **pRgb, DWORD *pcbRgb)
{
	CONST KERB_INTERACTIVE_LOGON *pObjKil = &objKiul.Logon;

	DWORD cb = sizeof(KERB_INTERACTIVE_UNLOCK_LOGON) + pObjKil->LogonDomainName.Length + pObjKil->UserName.Length + pObjKil->Password.Length;
	KERB_INTERACTIVE_UNLOCK_LOGON *pObjKiulOut = (KERB_INTERACTIVE_UNLOCK_LOGON*)CoTaskMemAlloc(cb);
	if(pObjKiulOut == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ZeroMemory(&pObjKiulOut->LogonId, sizeof(LUID));

	KERB_INTERACTIVE_LOGON *pObjKilOut = &pObjKiulOut->Logon;
	pObjKilOut->MessageType = pObjKil->MessageType;

	BYTE *pbBuffer = (BYTE*)pObjKiulOut + sizeof(KERB_INTERACTIVE_UNLOCK_LOGON);

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(pObjKil->LogonDomainName, (PWSTR)pbBuffer, &pObjKilOut->LogonDomainName);
	pObjKilOut->LogonDomainName.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKiulOut);
	pbBuffer += pObjKilOut->LogonDomainName.Length;

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(pObjKil->UserName, (PWSTR)pbBuffer, &pObjKilOut->UserName);
	pObjKilOut->UserName.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKiulOut);
	pbBuffer += pObjKilOut->UserName.Length;

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(pObjKil->Password, (PWSTR)pbBuffer, &pObjKilOut->Password);
	pObjKilOut->Password.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKiulOut);

	*pRgb = (BYTE*)pObjKiulOut;
	pObjKiulOut = NULL;
	*pcbRgb = cb;

	return S_OK;
}

HRESULT SSWACPTLSAInitString(STRING *lpDes, LPCSTR lpSrc)
{
	
	HRESULT hRes = E_FAIL;

	size_t cchLength = 0;
	hRes = ::StringCchLengthA(lpSrc, USHORT_MAX, &cchLength);
	if(FAILED(hRes))
	{
		return hRes;
	}

	USHORT usLen = 0;
	hRes = SizeTToUShort(cchLength, &usLen);
	if(FAILED(hRes))
	{
		return hRes;
	}

	lpDes->Buffer = (PCHAR)lpSrc;
	lpDes->Length = usLen;
	lpDes->MaximumLength = lpDes->Length + 1;

	return S_OK;

}

HRESULT SSWACPTRetrieveNegotiateAuthPackage(ULONG *pcbAuthPackage)
{
	HANDLE hLsa = NULL;

	check_HRESULT_begin
	{
		check_HRESULT_bool(pcbAuthPackage != NULL, E_INVALIDARG);

		check_HRESULT_SUCCEEDED(HRESULT_FROM_NT(::LsaConnectUntrusted(&hLsa)));

		ULONG ulAuthPackage = 0;
		LSA_STRING lsaszkerberosName;
		check_HRESULT_SUCCEEDED(SSWACPTLSAInitString(&lsaszkerberosName, NEGOSSP_NAME_A));

		check_HRESULT_SUCCEEDED(HRESULT_FROM_NT(::LsaLookupAuthenticationPackage(hLsa, &lsaszkerberosName, &ulAuthPackage)));

		*pcbAuthPackage = ulAuthPackage;
	}
	check_HRESULT_finally
	{

	}

	if(hLsa != NULL)
	{
		::LsaDeregisterLogonProcess(hLsa);
		hLsa = NULL;
	}

	check_HRESULT_return;
}

HRESULT SSWACPSpliterDomainAndUsername(LPCWSTR lpDomainUsername, LPWSTR lpDomain, DWORD cchDomain, LPWSTR lpUsername, DWORD cchUsername)
{
	if(lpDomainUsername == NULL || _tcslen(lpDomainUsername) <= 0)
	{
		return E_INVALIDARG;
	}

	if( (lpDomain != NULL && cchDomain <= 0) ||
		(lpUsername != NULL && cchUsername <= 0) )
	{
		return E_INVALIDARG;
	}

	LPCWSTR lpUserNameBegin = wcschr(lpDomainUsername, L'\\');

	if(lpDomain != NULL)
	{
		if(lpUserNameBegin == NULL)
		{
			if(!::GetComputerNameW(lpDomain, &cchDomain))
			{
				return E_FAIL;
			}
		}else
		{
			DWORD cchLen= (DWORD)(lpUserNameBegin - lpDomainUsername);
			if(cchLen >= cchDomain)
			{
				return E_INVALIDARG;
			}
			wcsncpy_s(lpDomain, cchDomain, lpDomainUsername, cchLen);
			lpDomain[cchLen] = L'\0';
		}
	}

	if(lpUsername != NULL)
	{
		if(lpUserNameBegin == NULL)
		{
			lpUserNameBegin = lpDomainUsername;
		}else
			lpUserNameBegin++;
		if(wcslen(lpUserNameBegin) >= cchUsername)
		{
			return E_INVALIDARG;
		}
		_tcscpy_s(lpUsername, cchUsername, lpUserNameBegin);
	}

	return S_OK;
}

HRESULT SSWACPTKerbChangePasswordRequestInitW(LPWSTR lpDomain, LPWSTR lpUsername, LPWSTR lpOldPassword, LPWSTR lpNewPassword
											  , KERB_CHANGEPASSWORD_REQUEST * pObjKcr)
{
	check_HRESULT_begin
	{
		KERB_CHANGEPASSWORD_REQUEST objKcr;
		ZeroMemory(&objKcr, sizeof(objKcr));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpDomain, &objKcr.DomainName));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpUsername, &objKcr.AccountName));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpOldPassword, &objKcr.OldPassword));
		check_HRESULT(SSWACPTUnicodeStringInitWithStringW(lpNewPassword, &objKcr.NewPassword));

		objKcr.MessageType = KerbChangePasswordMessage;
		objKcr.Impersonating = FALSE;

		CopyMemory(pObjKcr, &objKcr, sizeof(objKcr));

	}
	check_HRESULT_finally
	{

	}

	check_HRESULT_return;
}

HRESULT SSWACPTKerbChangePasswordRequestPack(CONST KERB_CHANGEPASSWORD_REQUEST &objKcr, BYTE **pRgb, DWORD *pcbRgb)
{
	DWORD cb = sizeof(KERB_CHANGEPASSWORD_REQUEST) + objKcr.DomainName.Length + objKcr.AccountName.Length + objKcr.OldPassword.Length
		+ objKcr.NewPassword.Length;
	KERB_CHANGEPASSWORD_REQUEST *pObjKcr = (KERB_CHANGEPASSWORD_REQUEST*)CoTaskMemAlloc(cb);
	if(pObjKcr == NULL)
	{
		return E_OUTOFMEMORY;
	}
	pObjKcr->MessageType = objKcr.MessageType;
	pObjKcr->Impersonating = objKcr.Impersonating;

	BYTE *pbBuffer = (BYTE*)pObjKcr + sizeof(KERB_CHANGEPASSWORD_REQUEST);

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(objKcr.DomainName, (PWSTR)pbBuffer, &pObjKcr->DomainName);
	pObjKcr->DomainName.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKcr);
	pbBuffer += pObjKcr->DomainName.Length;

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(objKcr.AccountName, (PWSTR)pbBuffer, &pObjKcr->AccountName);
	pObjKcr->AccountName.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKcr);
	pbBuffer += pObjKcr->AccountName.Length;

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(objKcr.OldPassword, (PWSTR)pbBuffer, &pObjKcr->OldPassword);
	pObjKcr->OldPassword.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKcr);
	pbBuffer += pObjKcr->OldPassword.Length;

	SSWACPTUnicodeStringInitWithUnicodeStringCopy(objKcr.NewPassword, (PWSTR)pbBuffer, &pObjKcr->NewPassword);
	pObjKcr->NewPassword.Buffer = (PWSTR)(pbBuffer - (BYTE*)pObjKcr);

	*pRgb = (BYTE*)pObjKcr;
	pObjKcr = NULL;
	*pcbRgb = cb;

	return S_OK;
}
