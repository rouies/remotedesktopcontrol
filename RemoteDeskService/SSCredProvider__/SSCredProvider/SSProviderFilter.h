#pragma once

HRESULT CCProviderFilter_CreateInstance(REFIID riid, void **ppv);

class CSSProviderFilter : public ICredentialProviderFilter
{
public:
	CSSProviderFilter();
	virtual ~CSSProviderFilter();

public:
	// IUnknown
	ISSUnknown_REF_DEFINE;

	// ICredentialProviderFilter
	STDMETHOD(Filter)(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags, GUID* rgclsidProviders, BOOL* rgbAllow, DWORD cProviders);
	STDMETHOD(UpdateRemoteCredential)(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn
		, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut);

};