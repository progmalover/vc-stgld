// Browsers.h: interface for the CBrowsers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSERS_H__2BDC1427_002F_11D2_820F_5254AB10C899__INCLUDED_)
#define AFX_BROWSERS_H__2BDC1427_002F_11D2_820F_5254AB10C899__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CBrowsers  
{
protected:
	CBrowsers();
	virtual ~CBrowsers();
	
public:
	DECLARE_SINGLETON(CBrowsers)

	UINT GetBrowserNum();
    CString GetBrowserExeFile(int ExeIndex);
    CString GetBrowserName(int NameIndex);

	UINT GetSpecifyBrowserNum();
	CString GetSpecifyBrowserExeFile(int ExeIndex);
    CString GetSpecifyBrowserName(int NameIndex);
	
	BOOL AddSpecifyBrowser(const CString & Path,const CString & ExeName);
	BOOL DelSpecifyBrowser(const CString & ExeName);

private:
	CString GetNetscape3ExeFileName();
	CString GetNetscape4ExeFileName();
	void InitBrowserList();
	CString GetRegistryString(HKEY hKey,
					  const CString& Path,
					  const CString& ValueName);
	BOOL SetRegistryString(const CString & Path,
					       const CString & ValueName,
					       const CString & Value);
	CString GetIEExeFileName();
	CString GetOperaExeFileName();
	CString GetHotJavaExeFileName();
	void GetSpecifyBrowser();
	
	CString m_Current;
    CStringArray m_NetScaName,m_Net45Name;
	CStringArray saBrowserExeFile;
	CStringArray saBrowserName;
	UINT m_nBrowserNum,m_SpecifyNum;
	CStringArray m_SpecifyName,m_SpecifyPath;
};
 
#endif // !defined(AFX_BROWSERS_H__2BDC1427_002F_11D2_820F_5254AB10C899__INCLUDED_)

