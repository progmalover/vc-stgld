#pragma once

#include "Utils.h"
#include <vector>
#include <map>

class CParamInfo
{
public:
	CParamInfo(LPCTSTR lpszName, LPCTSTR lpszType, bool bOptional)
	{
		m_strName = lpszName;
		m_strType = lpszType;
		m_bOptional = bOptional;
	}
	~CParamInfo()
	{
	}
	CString m_strName;	// parameter name
	CString m_strType;	// parameter type
	bool m_bOptional;
};
typedef std::vector<CParamInfo *> PARAM_LIST;

class CFuncInfo
{
public:
	CFuncInfo(LPCTSTR lpszName, LPCTSTR lpszType, LPCTSTR lpszDesc)
	{
		m_strName = lpszName;
		m_strType = lpszType;
		m_strDesc = lpszDesc;
	}
	~CFuncInfo()
	{
		for (PARAM_LIST::iterator it = m_aParams.begin(); it != m_aParams.end(); it++)
			delete *it;
		m_aParams.clear();
	}

	CString m_strName;	// function name
	CString m_strType;	// return type
	CString m_strDesc;	// function description
	PARAM_LIST m_aParams;

	void AddParam(CParamInfo *pParamInfo)
	{
		m_aParams.push_back(pParamInfo);
	}
};
typedef std::map<CString, CFuncInfo*> FUNC_LIST;

class CEventInfo
{
public:
	CEventInfo(LPCTSTR lpszName, const CPoint &ptStart, const CPoint &ptEnd)
	{
		m_strName = lpszName;
		m_ptStart = ptStart;
		m_ptEnd = ptEnd;
	}
	~CEventInfo()
	{
	}

	CString m_strName;	// event name
	CPoint m_ptStart;
	CPoint m_ptEnd;
};
typedef std::map<CString, CEventInfo*> EVENT_LIST;

class CVarInfo
{
public:
	CVarInfo(LPCTSTR lpszName, LPCTSTR lpszType)
	{
		m_strName = lpszName;
		m_strType = lpszType;
	}
	~CVarInfo()
	{
	}

	CString m_strName;	// var name
	CString m_strType;	// var type
};
typedef std::map<CString, CVarInfo*> VAR_LIST;

class CClassInfo
{
public:
	CClassInfo();
	~CClassInfo();

public:
	CString m_strName;

	FUNC_LIST m_aFuncs;
	EVENT_LIST m_aEvents;
	VAR_LIST m_aVars;

	void AddFunc(CFuncInfo *pFunc);
	void RemoveAllFuncs();
	CFuncInfo *FindFunc(LPCTSTR lpszName);

	void AddEvent(CEventInfo *pEvent);
	void RemoveAllEvents();
	CEventInfo *FindEvent(LPCTSTR lpszName);

	void AddVar(CVarInfo *pVar);
	void RemoveAllVars();
	CVarInfo *FindVar(LPCTSTR lpszName);


	BOOL ParseFile(LPCTSTR lpszFile);
	BOOL ParseBuffer(LPCTSTR lpszBuf);
};
typedef std::map<CString, CClassInfo*> CLASS_LIST;

class CASParser
{
public:
	CASParser();
	~CASParser();

public:
	CLASS_LIST m_aClasses;
	FUNC_LIST m_aFuncs;
	VAR_LIST m_aVars;

	void AddFunc(CFuncInfo *pFunc);
	void RemoveAllFuncs();
	CFuncInfo *FindFunc(LPCTSTR lpszName);

	void AddVar(CVarInfo *pVar);
	void RemoveAllVars();
	CVarInfo *FindVar(LPCTSTR lpszName);

	void AddClass(CClassInfo *pClass);
	void RemoveAllClasses();
	CClassInfo *FindClass(LPCTSTR lpszName);

	BOOL ParseFile(LPCTSTR lpszFile);
	BOOL ParseBuffer(LPCTSTR lpszBuf);
};

extern CASParser g_ASParser;
