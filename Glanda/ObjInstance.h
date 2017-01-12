#pragma once
#include "gldMatrix.h"
#include "gldCxform.h"

class gldObj;

struct ObjInstance
{
    gldObj			*m_obj;
    gldMatrix		m_mx;
    gldCxform		m_cx;
	int				m_order;
	unsigned char	m_tag;

	ObjInstance() : m_obj(NULL), m_order(0), m_tag(0)
	{
	
	}

	ObjInstance(gldObj *pObj, const gldMatrix &mx, const gldCxform &cx, int order, unsigned char tag = 0)
	: m_obj(pObj), m_mx(mx), m_cx(cx), m_order(order), m_tag(tag)
	{

	}
};

typedef list<ObjInstance> OBJINSTANCE_LIST;
typedef list<ObjInstance>::iterator OBJINSTANCE_LIST_IT;