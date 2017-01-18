#pragma once

class ISSCredentialProviderCredential : public ICredentialProviderCredential
{
public:
	virtual HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD cpusFlags,
		ICredentialProvider *pCredentialProvider) = 0;
};