#pragma once


#include <wincred.h>
#include <ntsecapi.h>

HRESULT SSWACPTCopyStrProtect(LPWSTR lpStr, LPWSTR lpProtectedStr, DWORD *pCchProtectedStr);
HRESULT SSWACPTCopyStrProtectIfNecessaryW(LPWSTR lpStr, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, LPWSTR lpProtectedStr, DWORD *pCchProtectedStr);

HRESULT SSWACPTUnicodeStringInitWithStringW(LPWSTR lpStr, UNICODE_STRING *pStrUs);
HRESULT SSWACPTKerbInteractiveUnlockLogonInitW(LPWSTR lpDomain, LPWSTR lpUsername, LPWSTR lpPassword, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus
											   , KERB_INTERACTIVE_UNLOCK_LOGON *pObjKiul);

VOID SSWACPTUnicodeStringInitWithUnicodeStringCopy(CONST UNICODE_STRING& rus, PWSTR lpBuffer, UNICODE_STRING *pus);
HRESULT SSWACPTKerbInteractiveUnlockLogonPack(CONST KERB_INTERACTIVE_UNLOCK_LOGON &objKiul, BYTE **pRgb, DWORD *pcbRgb);

HRESULT SSWACPTRetrieveNegotiateAuthPackage(ULONG *pcbAuthPackage);

HRESULT SSWACPSpliterDomainAndUsername(LPCWSTR lpDomainUsername, LPWSTR lpDomain, DWORD cchDomain, LPWSTR lpUsername, DWORD cchUsername);

HRESULT SSWACPTKerbChangePasswordRequestInitW(LPWSTR lpDomain, LPWSTR lpUsername, LPWSTR lpOldPassword, LPWSTR lpNewPassword
											  , KERB_CHANGEPASSWORD_REQUEST * pObjKcr);
HRESULT SSWACPTKerbChangePasswordRequestPack(CONST KERB_CHANGEPASSWORD_REQUEST &objKcr, BYTE **pRgb, DWORD *pcbRgb);

