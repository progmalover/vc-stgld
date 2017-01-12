#pragma once

#include "..\CommonDef\CommonDef.h"

// CGLD_PropertyPage class

class AFX_NOVTABLE CGLD_PropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CGLD_PropertyPage)

// Constructors
public:
	CGLD_PropertyPage(UINT nIDD, UINT nIDCaption);

// Destructors
	virtual ~CGLD_PropertyPage() = 0;

protected:
	virtual void OnFinalRelease();

private:
	UINT m_nIDDialog;
	UINT m_nIDCaption;
	CString m_strCaption;
	SIZE m_sizePage;
	HGLOBAL m_hDialog;          // Handle of the dialog resource
	BOOL Init();

protected:
	CComPtr<IUnknown> m_pIUnknown;

// Interface Maps
public:
	BEGIN_INTERFACE_PART(PropertyPage, IGLD_PropertyPage)
		INIT_INTERFACE_PART(CGLD_PropertyPage, PropertyPage)
		STDMETHOD(GetTitle)(LPOLESTR *ppszTitle);
		STDMETHOD(GetSize)(SIZE *pSize);
		STDMETHOD(SetObject)(IUnknown *pUnk);
		STDMETHOD(Create)(HWND hWndParent, LPCRECT pRect);
		STDMETHOD(Activate)(BOOL bActivate);
		STDMETHOD(Enable)(BOOL bEnable);
		STDMETHOD(Show)(BOOL bShow);
		STDMETHOD(SetFocus)(void);
		STDMETHOD(Move)(LPCRECT pRect);
		STDMETHOD(Apply)(void);
	END_INTERFACE_PART(PropertyPage)

	DECLARE_INTERFACE_MAP()

protected:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
public:
	virtual BOOL OnInitDialog();
};
