#include "stdafx.h"
#include "SSFactory.h"
#include "SSProvider.h"
#include "SSProviderFilter.h"
//
//
//
ISSUnknown_REF_IMPL(CSSFactory)
CSSFactory::CSSFactory()
{
	ISSUnknown_REF_INIT;
}

CSSFactory::~CSSFactory()
{

}

STDMETHODIMP CSSFactory::QueryInterface(REFIID riid, void** ppv)
{
	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(riid != IID_IUnknown && riid != IID_IClassFactory)
		return E_NOINTERFACE;

	*ppv = static_cast<IUnknown*>(this);
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

STDMETHODIMP CSSFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
	if(ppv == NULL)
		return E_INVALIDARG;

	*ppv = NULL;

	if(pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	HRESULT hRes = E_FAIL;
	hRes = CSSProvider_CreateInstance(riid, ppv);
	if(hRes == S_OK)
		return hRes;

	return CCProviderFilter_CreateInstance(riid, ppv);
}

STDMETHODIMP CSSFactory::LockServer(BOOL bLock)
{
	bLock ? SSWACredProvAddRef() : SSWACredProvRelease();
	return S_OK;
}

//
//
//
HRESULT CSSFactory_CreateInstance(REFCLSID rclsid, REFIID riid, void **ppv)
{
	if(rclsid != CLSID_SSCredProvider)
		return CLASS_E_CLASSNOTAVAILABLE;

	CSSFactory *pCredProvFactory = new CSSFactory;
	if(pCredProvFactory == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hRes = pCredProvFactory->QueryInterface(riid, ppv);

	pCredProvFactory->Release();

	return hRes;
}