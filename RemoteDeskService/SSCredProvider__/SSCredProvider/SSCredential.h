#pragma once


#include "SSCredentialInterface.h"
#include "SSProvider.h"
#include <string>

enum SSCred_FIELD_ID
{
	SSFI_BEGIN					= 0,
	SSFI_TITLEIMAGE				= SSFI_BEGIN,
	SSFI_PASSWORD				= 1,
	SSFI_PIN					= 2,
	SSFI_USERNAME				= 3,
	SSFI_NEWPASSWORD		    = 4,
	SSFI_NEWPASSWORD_CONFIRM	= 5,
	SSFI_LARGE_TEXT				= 6,
	SSFI_SMALL_TEXT				= 7,
	SSFI_SUBMIT_BUTTON			= 8,
	SSFI_COMMAND_LINK			= 9,	
	SSFI_NUM_FILEDS				= 10
};

class CSSCredential : public ISSCredentialProviderCredential
{
public:
	CSSCredential();
	~CSSCredential();

public:
	// IUnknown
	ISSUnknown_REF_DEFINE;

	// ICredentialProviderCredential
	STDMETHOD(Advise)(ICredentialProviderCredentialEvents *pcpce);
	STDMETHOD(UnAdvise)();

	STDMETHOD(SetSelected)(BOOL *pbAutoLogon);
	STDMETHOD(SetDeselected)();

	STDMETHOD(GetFieldState)(DWORD index, CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs
		, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis);

	STDMETHOD(GetStringValue)(DWORD index, PWSTR *ppwsz);
	STDMETHOD(GetBitmapValue)(DWORD index, HBITMAP *phbmp);
	STDMETHOD(GetCheckboxValue)(DWORD index, BOOL *pbChecked, PWSTR *ppwszLabel);
	STDMETHOD(GetComboBoxValueCount)(DWORD index, DWORD *pcItems, DWORD *pdwSelectedItem);
	STDMETHOD(GetComboBoxValueAt)(DWORD index, DWORD item, PWSTR *ppwszItem);
	STDMETHOD(GetSubmitButtonValue)(DWORD index, DWORD *pdwAdjacentTo);

	STDMETHOD(SetStringValue)(DWORD index, PCWSTR pwz);
	STDMETHOD(SetCheckboxValue)(DWORD index, BOOL bChecked);
	STDMETHOD(SetComboBoxSelectedValue)(DWORD index, DWORD selItem);

	STDMETHOD(CommandLinkClicked)(DWORD index);

	STDMETHOD(GetSerialization)(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr
		, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs
		, PWSTR *ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi);
	STDMETHOD(ReportResult)(NTSTATUS status, NTSTATUS subStatus, PWSTR *ppwszOptionalStatusText
		, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsi);

public:
	virtual HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD cpusFlags, ICredentialProvider *pCredentialProvider);

public:
	HRESULT TryDoLogon(LPCWSTR lpDomain, LPCWSTR lpUsername, LPCWSTR lpPassword, BOOL fNotifyProvider = TRUE);
	
private:
	CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;
	DWORD m_dwCredUIFlags;
	ICredentialProvider *m_pCredentialProvider;

	ICredentialProviderCredentialEvents *m_pcpce;

	LPWSTR m_pwszFieldStrings[SSFI_NUM_FILEDS];

	SSWACredProvCredentialInfos m_CredentialInfos;
	int loginSuccess;
	
};

