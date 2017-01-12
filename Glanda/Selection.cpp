#include "StdAfx.h"
#include "selection.h"
#include "gldCharacter.h"
#include "gldLayer.h"
#include "SWFProxy.h"
#include "my_app.h"

#include "gldInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCharacterKeyList::CCharacterKeyList()
{
}

CCharacterKeyList::CCharacterKeyList(const CCharacterKeyList &other)
{
	clear();

	for (const_iterator i = other.begin(); i != other.end(); ++i)
	{
		push_back(*i);
	}
}

CCharacterKeyList::CCharacterKeyList(const gld_shape_sel &sel)
{
	clear();

	gld_shape_sel::iterator i = sel.begin();

	for (; i != sel.end(); ++i)
	{
		ASSERT((gldCharacterKey *)CTraitCharacter(*i));	

		push_back(CTraitCharacter(*i));
	}
}

CCharacterKeyList &CCharacterKeyList::operator=(const CCharacterKeyList &other)
{	
	if (&other != this)
	{
		clear();

		for (const_iterator i = other.begin(); i != other.end(); ++i)
		{
			push_back(*i);
		}
	}

	return *this;
}

bool CCharacterKeyList::operator==(const CCharacterKeyList &other) const
{
	if (size() == other.size())
	{
		for (const_iterator i = begin(); i != end(); ++i)
		{
			if (!other.exist(*i))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool CCharacterKeyList::operator!=(const CCharacterKeyList &other) const
{
	if (size() == other.size())
	{
		for (const_iterator i = begin(); i != end(); ++i)
		{
			if (!exist(*i))
			{
				return true;
			}
		}

		return false;
	}

	return true;
}

CCharacterKeyList::operator gld_shape_sel()
{
	gld_shape_sel	sel;

	for (iterator i = begin(); i != end(); ++i)
	{
		gld_shape	shape = CTraitShape(*i);
		
		ASSERT(shape.validate());

		sel.select(shape);
	}

	return sel;
}

bool CCharacterKeyList::exist(gldCharacterKey *key) const
{
	for (const_iterator i = begin(); i != end(); ++i)
	{
		if (*i == key)
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

CGuardSelKeeper::CGuardSelKeeper()
{
	gld_shape_sel &sel = my_app.CurSel();
	for (gld_shape_sel::iterator it = sel.begin(); it != sel.end(); it++)
		m_instanceList.push_back(CTraitInstance(*it));

	m_bSubmit = true;
}

CGuardSelKeeper::~CGuardSelKeeper()
{
	Submit();
}

void CGuardSelKeeper::Submit()
{
	if (m_bSubmit)
	{
		GINSTANCE_LIST::iterator i = m_instanceList.begin();
		while (i != m_instanceList.end())
		{
			if ((*i)->m_ptr == NULL)
			{
				m_instanceList.erase(i++);
			}
			else
			{
				i++;
			}
		}	

		gld_shape_sel sel;
		for (GINSTANCE_LIST::iterator it = m_instanceList.begin(); it != m_instanceList.end(); it++)
			sel.select(CTraitShape(*it));

		my_app.DoSelect(sel);

		m_bSubmit = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

CGuardSelAppend::CGuardSelAppend()
{
	m_bSubmit = true;

	gld_frame	frame = my_app.CurFrame();

	for (gld_layer_iter ilayer = frame.begin_layer(); ilayer != frame.end_layer(); ++ilayer)
	{
		for (gld_shape_iter ishape = (*ilayer).begin_shape(); ishape != (*ilayer).end_shape(); ++ishape)
		{
			if ((*ishape).ptr() != NULL)
				m_instanceList.push_back(CTraitInstance(*ishape));
		}
	}
}

CGuardSelAppend::~CGuardSelAppend()
{
	Submit();
}

void CGuardSelAppend::Submit()
{
	if (m_bSubmit)
	{
		gld_shape_sel	sel;

		gld_frame	frame = my_app.CurFrame();

		for (gld_layer_iter ilayer = frame.begin_layer(); ilayer != frame.end_layer(); ++ilayer)
		{
			for (gld_shape_iter ishape = (*ilayer).begin_shape(); ishape != (*ilayer).end_shape(); ++ishape)
			{
				if ((*ishape).ptr() != NULL)
				{
					if (find(m_instanceList.begin(), m_instanceList.end(), (gldInstance *)CTraitInstance(*ishape)) == m_instanceList.end())
					{
						sel.select(*ishape);
					}
				}				
			}
		}

		if (!sel.empty())	// no append
		{
			my_app.DoSelect(sel);
		}

		m_bSubmit = false;
	}
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

CGuardSelGuidLine::CGuardSelGuidLine(tGuidLine *pGuidLine, int nType)
{
	m_pLayer = CSWFProxy::GetCurLayer();
	m_pCurSel = my_app.GetGuidLine();
	m_pGuidLine = pGuidLine;
	m_nType = nType;
	m_bSubmit = true;
}

CGuardSelGuidLine::~CGuardSelGuidLine()
{
	Submit();
}

void CGuardSelGuidLine::Submit()
{
	if (m_bSubmit && (m_pLayer == CSWFProxy::GetCurLayer()))
	{
		switch (m_nType)
		{
		case UPDATE_APPEND:			
			my_app.SelectGuidLine(m_pGuidLine);			
			break;
		case UPDATE_REMOVE:
			my_app.SelectGuidLine(m_pCurSel == m_pGuidLine ? NULL : m_pCurSel);
			break;
		case UPDATE_MODIFY:
			my_app.SelectGuidLine(m_pGuidLine);
			break;
		default:
			ASSERT(false);
		}

		m_bSubmit = false;
	}
}
