#pragma once
#include "command.h"
#include "SWFProxy.h"
#include "gld_graph.h"
#include "TransAdaptor.h"
#include "Observer.h"
#include <list>
#include "GlandaDoc.h"

using namespace std;

class CExtractObj
{
public:
	CExtractObj();
	CExtractObj(const gld_shape &shape);
	CExtractObj(gldMovieClip *pMC);

	operator gldObj*();

protected:
	gldObj	*m_pObj;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class _Base>
class CCmdUpdateObjTmpl : public _Base
{
public:
	CCmdUpdateObjTmpl(gldObj *pObj)
	: m_pObj(pObj)
	{
		GetRefObjList();
	}

	virtual ~CCmdUpdateObjTmpl()
	{
	}

	void GetRefObjList()
	{
		CSWFProxy::GetRefObjList(m_pObj, m_lstRefObj);
	}

protected:
	gldObj			*m_pObj;
	list<gldObj *>	m_lstRefObj;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class _Base>
class CCmdUpdateObjUniIdTmpl : public CCmdUpdateObjTmpl<_Base>
{
public:
	CCmdUpdateObjUniIdTmpl(gldObj *pObj)
	: CCmdUpdateObjTmpl<_Base>(pObj)
	{
		if (m_pObj)
		{
			m_UniId = _GetObjectMap()->GenUniId();
			for (list<gldObj *>::iterator i = m_lstRefObj.begin(); i != m_lstRefObj.end(); ++i)
			{
				m_lstUniId.push_back(_GetObjectMap()->GenUniId());
			}
		}
	}

	virtual ~CCmdUpdateObjUniIdTmpl()
	{
	}

	void UpdateUniId()
	{
		if (m_pObj)
		{
			swap(m_UniId, m_pObj->m_uniId);
			list<gldObj *>::iterator iObj = m_lstRefObj.begin();
			list<UniId>::iterator iId = m_lstUniId.begin();
			for (; iObj != m_lstRefObj.end(); ++iObj, ++iId)
			{
				ASSERT(iId != m_lstUniId.end());
				swap((*iObj)->m_uniId, *iId);
			}
		}
	}

	void DupUniId(CCmdUpdateObjUniIdTmpl<_Base> *other)
	{
		m_UniId = other->m_UniId;
		ASSERT(m_lstUniId.size() == other->m_lstUniId.size());
		m_lstUniId.clear();
		list<UniId>::iterator i = other->m_lstUniId.begin();
		for (; i != other->m_lstUniId.end(); i++)
		{
			m_lstUniId.push_back(*i);
		}
	}
protected:
	UniId			m_UniId;
	list<UniId>		m_lstUniId;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class _Base>
class CCmdReshapeObjTmpl : public CCmdUpdateObjUniIdTmpl<_Base>
{
public:
	CCmdReshapeObjTmpl(gldObj *pObj)
	: CCmdUpdateObjUniIdTmpl<_Base>(pObj)
	{
	}

	~CCmdReshapeObjTmpl()
	{
	}

	void UpdateBoundBox()
	{
		ASSERT(m_pObj && m_pObj->IsGObjInstanceOf(gobjShape));
		ASSERT(m_pObj->m_ptr);

		gld_shape	tshape = *(gld_shape *)m_pObj->m_ptr;

		tshape.bound(tshape.calc_bound());
	}

	void UpdateRecord()
	{
		ASSERT(m_pObj && m_pObj->IsGObjInstanceOf(gobjShape));
		ASSERT(m_pObj->m_ptr);

		gld_shape	tshape = *(gld_shape *)m_pObj->m_ptr;	
		gldShape	*gshape = (gldShape *)m_pObj;

		CTransAdaptor::TShape2GShape(tshape, *gshape);
	}
};

typedef CCmdUpdateObjTmpl<TCommand>	CCmdUpdateObj;
typedef CCmdUpdateObjUniIdTmpl<TCommand>	CCmdUpdateObjUniId;
typedef CCmdReshapeObjTmpl<TCommand>	CCmdReshapeObj;

typedef CCmdUpdateObjTmpl<TCommandGroup>	CCmdGroupUpdateObj;
typedef CCmdUpdateObjUniIdTmpl<TCommandGroup>	CCmdGroupUpdateObjUniId;
typedef CCmdReshapeObjTmpl<TCommandGroup>	CCmdGroupReshapeObj;