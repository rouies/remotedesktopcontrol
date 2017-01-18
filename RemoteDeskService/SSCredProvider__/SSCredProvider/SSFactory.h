#pragma once

class CSSFactory : public IClassFactory
{
public:
	CSSFactory();
	~CSSFactory();

public:
	ISSUnknown_REF_DEFINE;

public:
	// IClassFactory
	STDMETHOD(CreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	STDMETHOD(LockServer)(BOOL bLock);

};

HRESULT CSSFactory_CreateInstance(REFCLSID rclsid, REFIID riid, void **ppv);
