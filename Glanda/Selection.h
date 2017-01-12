#pragma once

#include "gld_selection.h"
#include <list>
#include <algorithm>

using namespace std;

class gldCharacterKey;
class gldLayer;
struct tGuidLine;
class gldInstance;

class CCharacterKeyList : public list<gldCharacterKey *>
{
public:
	CCharacterKeyList();
	CCharacterKeyList(const CCharacterKeyList &other);
	CCharacterKeyList(const gld_shape_sel &sel);

	CCharacterKeyList &operator=(const CCharacterKeyList &other);

	bool operator==(const CCharacterKeyList &other) const;
	bool operator!=(const CCharacterKeyList &other) const;

	operator gld_shape_sel();

	bool exist(gldCharacterKey *key) const;
};

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

class CGuardSelKeeper
{
public:
	CGuardSelKeeper();
	~CGuardSelKeeper();
	void Submit();

protected:
	std::list<gldInstance *>	m_instanceList;	
	bool m_bSubmit;
};

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

class CGuardSelAppend
{
public:
	CGuardSelAppend();
	~CGuardSelAppend();
	void Submit();

protected:
	list<gldInstance *>	m_instanceList;
	bool m_bSubmit;
};

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

class CGuardSelGuidLine
{
public:
	enum
	{
		UPDATE_APPEND	=	1,
		UPDATE_REMOVE	=	2,
		UPDATE_MODIFY	=	3
	};

	CGuardSelGuidLine(tGuidLine *pGuidLine, int nType);
	~CGuardSelGuidLine();
	void Submit();

protected:
	gldLayer	*m_pLayer;
	tGuidLine	*m_pGuidLine;
	tGuidLine	*m_pCurSel;
	int			m_nType;
	bool		m_bSubmit;
};