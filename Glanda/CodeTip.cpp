#include "StdAfx.h"
#include ".\codetip.h"
#include "Utils.h"

#include "Regexx.h"
using namespace regexx;

CASParser g_ASParser;	// built-in functions, classes

void AddFunc(FUNC_LIST &aFuncs, CFuncInfo *pFunc)
{
	FUNC_LIST::iterator it = aFuncs.find(pFunc->m_strName);
	if (it != aFuncs.end())
		delete (*it).second;
	aFuncs[pFunc->m_strName] = pFunc;
}

void AddVar(VAR_LIST &aVars, CVarInfo *pVar)
{
	VAR_LIST::iterator it = aVars.find(pVar->m_strName);
	if (it != aVars.end())
		delete (*it).second;
	aVars[pVar->m_strName] = pVar;
}

BOOL ParseBuffer(LPCTSTR lpszBuf, FUNC_LIST &aFuncs, VAR_LIST &aVars)
{
	const TCHAR *pszStart = lpszBuf;
	while (true)
	{
		Regexx re;
		unsigned int ret = re.exec(pszStart, "function[ \t\r\n]+([a-zA-Z_][a-zA-Z0-9_]*)[ \t\r\n]*\\((.*?)\\)[ \t]*", Regexx::nocase | Regexx::dotall);
		if (ret > 0)
		{
			pszStart += re.match[0].start() + re.match[0].length();

			TRACE2("%s:%s\n", re.match[0].atom[0].str().c_str(), re.match[0].atom[1].str().c_str());

			// add a function
			CFuncInfo *pFunc = new CFuncInfo(re.match[0].atom[0].str().c_str(), "", "");

			Regexx reComment;
			ret = reComment.exec(pszStart, "^//[ \t]*(.*)");
			if (ret > 0)
			{
				pFunc->m_strDesc = reComment.match[0].atom[0].str().c_str();
				pFunc->m_strDesc.TrimRight("\r\n \t");
			}

			// parse params
			std::string sParams = re.match[0].atom[1].str();
			const TCHAR *pszParam = sParams.c_str();
			Regexx reParam;
			while (true)
			{
				while (*pszParam == ' ' || *pszParam == '\t' || *pszParam == '\r' || *pszParam == '\n')
					pszParam++;
				ret = reParam.exec(pszParam, "(.+?)[ \t\r\n]*(,|$)");
				if (ret > 0)
				{
					pszParam += reParam.match[0].start() + reParam.match[0].length();
					// add a param
					pFunc->AddParam(new CParamInfo(reParam.match[0].atom[0].str().c_str(), "", FALSE));
				}
				else
				{
					break;
				}
			}

			::AddFunc(aFuncs, pFunc);
		}
		else
		{
			break;
		}
	}

	pszStart = lpszBuf;
	while (true)
	{
		Regexx re;
		unsigned int ret = re.exec(pszStart, "var[ \t\r\n]+([a-zA-Z_][a-zA-Z0-9_]*)[ \t\r\n]*(\\:[ \t\r\n]*[a-zA-Z_][a-zA-Z0-9_]*|)", Regexx::nocase | Regexx::dotall);
		if (ret > 0)
		{
			pszStart += re.match[0].start() + re.match[0].length();

			std::string sName = re.match[0].atom[0].str();
			std::string sType = re.match[0].atom[1].str();

			// add a var
			const TCHAR *pszName =sName.c_str();
			const TCHAR *pszType = sType.c_str();
			if (!sType.empty())
				while (*pszType == ':' || *pszType == ' ' || *pszType == '\t' || *pszType == '\r' || *pszType == '\n')
					pszType++;
			CVarInfo *pVar = new CVarInfo(pszName, pszType);
			::AddVar(aVars, pVar);
		}
		else
		{
			break;
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// class CClassInfo
CClassInfo::CClassInfo()
{
}

CClassInfo::~CClassInfo()
{
	RemoveAllFuncs();
	RemoveAllVars();
}

void CClassInfo::AddFunc(CFuncInfo *pFunc)
{
	::AddFunc(m_aFuncs, pFunc);
}

CFuncInfo *CClassInfo::FindFunc(LPCTSTR lpszName)
{
	FUNC_LIST::iterator it = m_aFuncs.find(lpszName);
	if (it != m_aFuncs.end())
		return (*it).second;
	return NULL;
}

void CClassInfo::RemoveAllFuncs()
{
	for (FUNC_LIST::iterator it = m_aFuncs.begin(); it != m_aFuncs.end(); it++)
		delete (*it).second;
	m_aFuncs.clear();
}

void CClassInfo::AddVar(CVarInfo *pVar)
{
	::AddVar(m_aVars, pVar);
}

void CClassInfo::RemoveAllVars()
{
	for (VAR_LIST::iterator it = m_aVars.begin(); it != m_aVars.end(); it++)
		delete (*it).second;
	m_aVars.clear();
}

CVarInfo *CClassInfo::FindVar(LPCTSTR lpszName)
{
	VAR_LIST::iterator it = m_aVars.find(lpszName);
	if (it != m_aVars.end())
		return (*it).second;
	return NULL;
}

BOOL CClassInfo::ParseFile(LPCTSTR lpszFile)
{
	BOOL bResult = FALSE;
	TCHAR *pszBuf = NULL;
	try
	{
		CStdioFile file(lpszFile, CFile::typeBinary | CFile::modeRead | CFile::shareDenyWrite);
		int len = file.GetLength();
		pszBuf = new TCHAR[len + 1];
		if (pszBuf)
		{
			file.Read(pszBuf, len);
			pszBuf[len] = '\0';
		
			bResult = ParseBuffer(pszBuf);
		}
	}
	catch (CFileException *e)
	{
#ifdef _DEBUG
		e->ReportError();
#endif
		e->Delete();
	}

	if (pszBuf)
		delete pszBuf;

	return bResult;
}

BOOL CClassInfo::ParseBuffer(LPCTSTR lpszBuf)
{
	m_strName.Empty();
	RemoveAllFuncs();

	Regexx re;
	unsigned int ret = re.exec(lpszBuf, "class[ \t\r\n]+([a-zA-Z_][a-zA-Z0-9_]*)[ \t\r\n]*{.*}", Regexx::nocase | Regexx::dotall);
	if (ret == 0)
		return FALSE;

	m_strName = re.match[0].atom[0].str().c_str();

	return ::ParseBuffer(lpszBuf, m_aFuncs, m_aVars);
}

////////////////////////////////////////////////////////////
// class CASParser
CASParser::CASParser()
{
}

CASParser::~CASParser()
{
	RemoveAllClasses();
	RemoveAllFuncs();
	RemoveAllVars();
}

void CASParser::AddClass(CClassInfo *pClass)
{
	CLASS_LIST::iterator it = m_aClasses.find(pClass->m_strName);
	if (it != m_aClasses.end())
		delete (*it).second;
	m_aClasses[pClass->m_strName] = pClass;
}

CClassInfo *CASParser::FindClass(LPCTSTR lpszName)
{
	CLASS_LIST::iterator it = m_aClasses.find(lpszName);
	if (it != m_aClasses.end())
		return (*it).second;
	return NULL;
}

void CASParser::RemoveAllClasses()
{
	for (CLASS_LIST::iterator it = m_aClasses.begin(); it != m_aClasses.end(); it++)
		delete (*it).second;
	m_aClasses.clear();
}

void CASParser::RemoveAllFuncs()
{
	for (FUNC_LIST::iterator it = m_aFuncs.begin(); it != m_aFuncs.end(); it++)
		delete (*it).second;
	m_aFuncs.clear();
}

void CASParser::AddFunc(CFuncInfo *pFunc)
{
	::AddFunc(m_aFuncs, pFunc);
}

CFuncInfo *CASParser::FindFunc(LPCTSTR lpszName)
{
	FUNC_LIST::iterator it = m_aFuncs.find(lpszName);
	if (it != m_aFuncs.end())
		return (*it).second;
	return NULL;
}

void CASParser::AddVar(CVarInfo *pVar)
{
	::AddVar(m_aVars, pVar);
}

void CASParser::RemoveAllVars()
{
	for (VAR_LIST::iterator it = m_aVars.begin(); it != m_aVars.end(); it++)
		delete (*it).second;
	m_aVars.clear();
}

CVarInfo *CASParser::FindVar(LPCTSTR lpszName)
{
	VAR_LIST::iterator it = m_aVars.find(lpszName);
	if (it != m_aVars.end())
		return (*it).second;
	return NULL;
}

BOOL CASParser::ParseFile(LPCTSTR lpszFile)
{
	BOOL bResult = FALSE;
	TCHAR *pszBuf = NULL;
	try
	{
		CStdioFile file(lpszFile, CFile::typeBinary | CFile::modeRead | CFile::shareDenyWrite);
		int len = file.GetLength();
		pszBuf = new TCHAR[len + 1];
		if (pszBuf)
		{
			file.Read(pszBuf, len);
			pszBuf[len] = '\0';
		
			bResult = ParseBuffer(pszBuf);
		}
	}
	catch (CFileException *e)
	{
#ifdef _DEBUG
		e->ReportError();
#endif
		e->Delete();
	}

	if (pszBuf)
		delete pszBuf;

	return bResult;
}

BOOL CASParser::ParseBuffer(LPCTSTR lpszBuf)
{
	RemoveAllFuncs();
	return ::ParseBuffer(lpszBuf, m_aFuncs, m_aVars);
}
