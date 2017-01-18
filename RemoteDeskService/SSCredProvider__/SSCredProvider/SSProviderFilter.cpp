#include "stdafx.h"
#include "SSProviderFilter.h"

HRESULT CCProviderFilter_CreateInstance(REFIID riid, void **ppv)
{
	SS_AUTO_FNC_TRACE;

	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	CSSProviderFilter *pProviderFilter = new CSSProviderFilter;
	if(pProviderFilter == NULL)
		return E_OUTOFMEMORY;

	HRESULT hRes = pProviderFilter->QueryInterface(riid, ppv);

	pProviderFilter->Release();

	return hRes;
}

//
//
//
ISSUnknown_REF_IMPL(CSSProviderFilter)
CSSProviderFilter::CSSProviderFilter()
{
	SS_AUTO_FNC_TRACE;

	SSWACredProvAddRef();

	ISSUnknown_REF_INIT;

}

CSSProviderFilter::~CSSProviderFilter()
{
	SS_AUTO_FNC_TRACE;

	SSWACredProvRelease();
}


STDMETHODIMP CSSProviderFilter::QueryInterface(REFIID riid, void** ppv)
{
	SS_AUTO_FNC_TRACE;

	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(riid != IID_IUnknown && riid != IID_ICredentialProviderFilter)
		return E_NOINTERFACE;

	*ppv = static_cast<IUnknown*>(this);
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

STDMETHODIMP CSSProviderFilter::Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags, 
									   GUID* rgclsidProviders, BOOL* rgbAllow, DWORD cProviders)
{
	switch (cpus)
	{
	case CPUS_LOGON:
	case CPUS_UNLOCK_WORKSTATION:
		{
			int i = 0;
			for (i = 0; i < (int)cProviders; i++)
			{
				if (IsEqualGUID(rgclsidProviders[i], CLSID_SSCredProvider))
					break;
			}
			if(i != (int)cProviders)
			{
				int j = 0;
				for(j = 0; j < (int)cProviders; j++)
					rgbAllow[j] = FALSE;
				rgbAllow[i] = TRUE;
			}
		}
		return S_OK;
	case CPUS_CREDUI:
	case CPUS_CHANGE_PASSWORD:
		return E_NOTIMPL;
	default:
		return E_INVALIDARG;
	}
}

STDMETHODIMP CSSProviderFilter::UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsIn 
												   , CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* pcpcsOut)
{
	return E_NOTIMPL;
}
