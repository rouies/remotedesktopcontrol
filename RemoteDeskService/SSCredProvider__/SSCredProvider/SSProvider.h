#pragma once

#include "SSCredential.h"

//
HRESULT CSSProvider_CreateInstance(REFIID riid, void **ppv);

//
#define SSCRED_MAX_CREDENTIALS 10

class CSSProvider : public ICredentialProvider
{
public:
	CSSProvider();
	virtual ~CSSProvider();

public:
	// IUnknown
	ISSUnknown_REF_DEFINE;

public:
	// ICredentialProvider
	STDMETHOD(SetUsageScenario)(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD flags);
	STDMETHOD(SetSerialization)(CONST CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcs);

	STDMETHOD(Advise)(ICredentialProviderEvents *pcpe, UINT_PTR advContext);
	STDMETHOD(UnAdvise)();

	STDMETHOD(GetFieldDescriptorCount)(DWORD *pdwCount);
	STDMETHOD(GetFieldDescriptorAt)(DWORD index, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd);

	STDMETHOD(GetCredentialCount)(DWORD *pdwCount, DWORD *pdwDefault, BOOL *pfAutoLogonWithDefault);
	STDMETHOD(GetCredentialAt)(DWORD index, ICredentialProviderCredential **ppcpc);

public:
	HRESULT ChangeCredentials(ICredentialProviderCredential *pCredential);
	
protected:

	ISSCredentialProviderCredential *m_pCredentials[SSCRED_MAX_CREDENTIALS];
	DWORD m_dwCredentials;

	DWORD m_dwAutoLogonCredentialIndex;

	ICredentialProviderEvents *m_pcpe;
	UINT_PTR m_uAdviseContext;
};

