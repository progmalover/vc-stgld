// VersionInfo.h: interface for the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONINFO_H__C26CD0B9_EBB4_402F_8CCA_7BDD3034FE39__INCLUDED_)
#define AFX_VERSIONINFO_H__C26CD0B9_EBB4_402F_8CCA_7BDD3034FE39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVersionInfo : public OSVERSIONINFO  
{
public:
	static void ReleaseInstance();
	BOOL IsWin98orLater();
	static CVersionInfo * Instance();
	CVersionInfo();
	virtual ~CVersionInfo();

private:
	static CVersionInfo * m_pVersionInfo;
};

#endif // !defined(AFX_VERSIONINFO_H__C26CD0B9_EBB4_402F_8CCA_7BDD3034FE39__INCLUDED_)
