#pragma once

#include "gldObj.h"
#include <map>
#include <sstream>
using namespace std;

extern char *ext_res_symbolName;            // "Symbol"
class gldLibrary;
class gldImage;
class UniId;
class gldFont;

inline bool UniIdIsEmpty(const UniId &id)
{
	return id.procId == 0 || id.docId == 0 || id.libId == 0;
}

class CObjectMap
{
public:
	CObjectMap(void);
	virtual ~CObjectMap(void);

	inline ULONG Use(gldObj *pObj)
	{
		if (UniIdIsEmpty(pObj->m_uniId))
			UpdateUniId(pObj);

		pair<_ObjectMapIter, bool> ret = m_Map.insert(_ObjectEntry(pObj, _ObjectData()));
		
		return ++(*ret.first).second.nRef;
	}

	inline ULONG Unuse(gldObj *pObj)
	{
		_ObjectMapIter i = m_Map.find(pObj);

		if (i == m_Map.end())
			return (ULONG)-1;
		
		ASSERT((*i).second.nRef > 0);

		ULONG nRef = --(*i).second.nRef;

		if (nRef == 0)
			m_Map.erase(i);

		return nRef;
	}
	
	inline void GenerateNextSymbolName(string &name)
	{
		ostringstream os;
		ASSERT(ext_res_symbolName != NULL);
		m_nMaxId = __max(m_nMaxId, m_Map.size() + 1);
		os << ext_res_symbolName << " " << m_nMaxId;
		name = os.str();
		m_nMaxId++;
	}
	
	void FreeAllSymbol();
	void CopyTo(gldLibrary *pLibrary);
	gldObj *FindObjectByID(WORD wId);
	gldObj *FindObjectByUniid(const UniId &id);
	gldFont *FindFont(LPCTSTR fontName, bool isBold, bool isItalic, bool isWideCodes);

	static void FreeSymbol(gldObj **ppObj);

	struct _ObjectData
	{
		ULONG	nRef;

		_ObjectData() : nRef(0) {}
	};

	typedef map<gldObj *, _ObjectData> _ObjectMap;
	typedef _ObjectMap::value_type _ObjectEntry;
	typedef _ObjectMap::iterator _ObjectMapIter;

	_ObjectMap m_Map;

	bool FindShapesUseImage(gldImage *pImage, GOBJECT_LIST *pObjList);

	UniId GenUniId();
	void UpdateUniId(gldObj *pObj);

protected:
	ULONG	m_nMaxId;
	ULONG	m_nUniId;
};