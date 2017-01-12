// VersionInfo.cpp: implementation of the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVersionInfo *CVersionInfo::m_pVersionInfo = NULL;

CVersionInfo::CVersionInfo()
{

}

CVersionInfo::~CVersionInfo()
{

}

CVersionInfo * CVersionInfo::Instance()
{
	if (!m_pVersionInfo)
	{
		m_pVersionInfo = new CVersionInfo();
		m_pVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(m_pVersionInfo);
	}

	return m_pVersionInfo;
}

BOOL CVersionInfo::IsWin98orLater()
{
	return (dwMajorVersion > 4 || (dwMajorVersion == 4 && dwMinorVersion > 0));
}

void CVersionInfo::ReleaseInstance()
{
	if (m_pVersionInfo)
	{
		delete m_pVersionInfo;
		m_pVersionInfo = NULL;
	}
}
