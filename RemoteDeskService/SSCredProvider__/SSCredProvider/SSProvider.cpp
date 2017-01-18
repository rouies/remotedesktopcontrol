#include "stdafx.h"
#include "SSProvider.h"

//
//
//
static const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR G_FIELDDESCS[] =
{

	{ SSFI_TITLEIMAGE, CPFT_TILE_IMAGE, L"TitleImg" },
	{ SSFI_PASSWORD, CPFT_PASSWORD_TEXT, L"ÃÜÂë" },
	{ SSFI_PIN, CPFT_EDIT_TEXT, L"ÊÚÈ¨ÐòÁÐºÅ" },
	{ SSFI_USERNAME, CPFT_EDIT_TEXT, L"Username" },
	{ SSFI_NEWPASSWORD, CPFT_PASSWORD_TEXT, L"New Password" },
	{ SSFI_NEWPASSWORD_CONFIRM, CPFT_PASSWORD_TEXT, L"Confirm New Password" },
	{ SSFI_LARGE_TEXT, CPFT_LARGE_TEXT, L"Large Text" },
	{ SSFI_SMALL_TEXT, CPFT_SMALL_TEXT, L"Small Text" },
	{ SSFI_SUBMIT_BUTTON, CPFT_SUBMIT_BUTTON, L"Submit" },
	{ SSFI_COMMAND_LINK, CPFT_COMMAND_LINK, L"Command_link" },
	};
//
//
//
HRESULT CSSProvider_CreateInstance(REFIID riid, void **ppv)
{
	SS_AUTO_FNC_TRACE;

	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	CSSProvider *pProvider = new CSSProvider;
	if(pProvider == NULL)
		return E_OUTOFMEMORY;

	HRESULT hRes = pProvider->QueryInterface(riid, ppv);

	pProvider->Release();

	return hRes;
}

//
//
//
ISSUnknown_REF_IMPL(CSSProvider)
CSSProvider::CSSProvider()
{
	SS_AUTO_FNC_TRACE;

	SSWACredProvAddRef();

	ISSUnknown_REF_INIT;

	m_dwCredentials = 0;
	m_pCredentials[m_dwCredentials++] = new CSSCredential();

	m_pcpe = NULL;
	m_uAdviseContext = 0;

	m_dwAutoLogonCredentialIndex = MAXDWORD;

}

CSSProvider::~CSSProvider()
{
	SS_AUTO_FNC_TRACE;

	while(m_dwCredentials-- > 0)
	{
		if(m_pCredentials[m_dwCredentials] != NULL)
		{
			m_pCredentials[m_dwCredentials]->Release();
			m_pCredentials[m_dwCredentials] = 0;
		}
	}
	m_dwCredentials = 0;

	SSWACredProvRelease();
}

// IUnknown

STDMETHODIMP CSSProvider::QueryInterface(REFIID riid, void** ppv)
{
	SS_AUTO_FNC_TRACE;

	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(riid != IID_IUnknown && riid != IID_ICredentialProvider)
		return E_NOINTERFACE;

	*ppv = static_cast<IUnknown*>(this);
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

// ICredentialProvider
STDMETHODIMP CSSProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD flags)
{
	SS_AUTO_FNC_TRACE;

	HRESULT hRes = E_FAIL;

	switch (cpus)
	{
	case CPUS_UNLOCK_WORKSTATION:
	case CPUS_LOGON:
	case CPUS_CREDUI:
	case CPUS_CHANGE_PASSWORD:
		for(DWORD dwIdx = 0; dwIdx<m_dwCredentials; dwIdx++)
		{
			if(m_pCredentials[dwIdx] != NULL)
			{
				m_pCredentials[dwIdx]->Initialize(cpus, flags, this);
			}
		}
		hRes = S_OK;
		break;
	case CPUS_PLAP:
	default:
		hRes = E_NOTIMPL;
		break;
	}

	return hRes;
}

STDMETHODIMP CSSProvider::SetSerialization(CONST CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION* /*pcpcs*/)
{
	SS_AUTO_FNC_TRACE;
	return E_INVALIDARG;
}

STDMETHODIMP CSSProvider::Advise(ICredentialProviderEvents *pcpe, UINT_PTR advContext)
{
	SS_AUTO_FNC_TRACE;
	if(m_pcpe != NULL)
	{
		m_pcpe->Release();
		m_pcpe = NULL;
	}

	m_pcpe = pcpe;
	m_pcpe->AddRef();

	m_uAdviseContext = advContext;

	return S_OK;
}

STDMETHODIMP CSSProvider::UnAdvise()
{
	SS_AUTO_FNC_TRACE;
	if(m_pcpe != NULL)
	{
		m_pcpe->Release();
		m_pcpe = NULL;
	}
	m_uAdviseContext = 0;

	return S_OK;
}

STDMETHODIMP CSSProvider::GetFieldDescriptorCount(DWORD *pdwCount)
{
	SS_AUTO_FNC_TRACE;

	*pdwCount = SSFI_NUM_FILEDS;

	return S_OK;
}

STDMETHODIMP CSSProvider::GetFieldDescriptorAt(DWORD index, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd)
{
	SS_AUTO_FNC_TRACE;

	if(index < SSFI_BEGIN || index >= SSFI_NUM_FILEDS || ppcpfd == NULL)
	{
		return E_INVALIDARG;
	}

	*ppcpfd = NULL;

	CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *pcpfd = (CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*)CoTaskMemAlloc(
		sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR));
	if(pcpfd == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hRes = E_FAIL;

	CONST CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *pcpfdSrc = &G_FIELDDESCS[index];

	pcpfd->dwFieldID = pcpfdSrc->dwFieldID;
	pcpfd->cpft = pcpfdSrc->cpft;
	if(pcpfdSrc->pszLabel != NULL)
	{
		hRes = ::SHStrDupW(pcpfdSrc->pszLabel, &pcpfd->pszLabel);
	}else
	{
		pcpfd->pszLabel = NULL;
		hRes = S_OK;
	}

	if(SUCCEEDED(hRes))
	{
		*ppcpfd = pcpfd;
		pcpfd = NULL;
	}else
	{
		CoTaskMemFree(pcpfd);
		pcpfd = NULL;
	}

	return hRes;
}

STDMETHODIMP CSSProvider::GetCredentialCount(DWORD *pdwCount, DWORD *pdwDefault, BOOL *pfAutoLogonWithDefault)
{
	SS_AUTO_FNC_TRACE;
	if(pdwCount != NULL)
	{
		*pdwCount = m_dwCredentials;
	}

	if(m_dwAutoLogonCredentialIndex == MAXDWORD)
	{
		*pdwDefault = MAXDWORD;
		*pfAutoLogonWithDefault = FALSE;
	}else
	{
		*pdwDefault = m_dwAutoLogonCredentialIndex;
		*pfAutoLogonWithDefault = TRUE;
	}

	m_dwAutoLogonCredentialIndex = MAXDWORD;

	return S_OK;
}

STDMETHODIMP CSSProvider::GetCredentialAt(DWORD index, ICredentialProviderCredential **ppcpc)
{
	SS_AUTO_FNC_TRACE;

	if(index < 0 || index >= m_dwCredentials ||ppcpc == NULL)
		return E_INVALIDARG;

	if(m_pCredentials[index] == NULL)
		return E_POINTER;

	return m_pCredentials[index]->QueryInterface(IID_ICredentialProviderCredential, reinterpret_cast<void**>(ppcpc));
}

HRESULT CSSProvider::ChangeCredentials(ICredentialProviderCredential *pCredential)
{
	SS_AUTO_FNC_TRACE;
	if(pCredential == NULL)
	{
		SS_LOG_V_W(CLSHT("pCredential is NULL, Skip This Change"));
		return E_INVALIDARG;
	}
	if(m_dwAutoLogonCredentialIndex != MAXDWORD)
	{
		SS_LOG_V_W(CLSHT("m_dwAutoLogonCredentialIndex = %d, Skip This Change"), m_dwAutoLogonCredentialIndex);
		return STG_E_INUSE;
	}

	if(m_pcpe == NULL)
	{
		SS_LOG_V_W(CLSHT("ICredentialProviderEvents Pointer is NULL, Skip This Change"));
		return E_FAIL;
	}

	m_dwAutoLogonCredentialIndex = MAXDWORD;

	if(pCredential != NULL)
	{
		DWORD ind = 0;
		for (ind=0; ind<m_dwCredentials; ind++)
		{
			if(m_pCredentials[ind] == pCredential)
			{
				m_dwAutoLogonCredentialIndex =ind;
			}
		}
	}
	//ÇÐ»»ÓÃ»§
	return m_pcpe->CredentialsChanged(m_uAdviseContext);
	
}
