#include "StdAfx.h"
#include "cmdupdateobj.h"
#include "TransAdaptor.h"
#include "gldCharacter.h"
#include "gldMovieClip.h"
#include "gldShape.h"

#include "SWFProxy.h"
#include "gldInstance.h"

CExtractObj::CExtractObj()
: m_pObj(NULL)
{
}

CExtractObj::CExtractObj(const gld_shape &shape)
{
	gld_shape tshape = shape;
	//ASSERT(tshape.ptr());
	//gldCharacter *pChar = (gldCharacter *)tshape.ptr();	
	//m_pObj = pChar->m_key->GetObj();

	gldInstance *pInstance = CTraitInstance(tshape);
	m_pObj = pInstance->m_obj;
}

CExtractObj::CExtractObj(gldMovieClip *pMC)
{
	m_pObj = NULL;
}

CExtractObj::operator gldObj*()
{
	return m_pObj;
}

