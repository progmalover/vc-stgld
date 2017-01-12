#include "StdAfx.h"
#include ".\background.h"
#include "gldObj.h"
#include "TransAdaptor.h"
#include "GlandaDoc.h"
#include "SWFProxy.h"
#include "my_app.h"
#include "..\GlandaCOM\GLD_ATL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBackground::CBackground(void)
: m_Pos(POS_CUSTOM)
{
	m_pInst = new gldInstance;
}

CBackground::CBackground(gldInstance *pInst, int pos)
: m_pInst(pInst)
, m_Pos(pos)
{
}

CBackground::CBackground(gldObj *pObj, int pos, const gldMatrix &mx, const gldCxform &cx)
: m_Pos(pos)
{
	ASSERT(pObj != NULL);
	m_pInst = new gldInstance(pObj, mx, cx);
}

CBackground::~CBackground(void)
{
	if (m_pInst != NULL)
		delete m_pInst;
	DestroyShape();	
}

gld_shape CBackground::GetEditShape(bool update)
{
	if (update || !m_Shape.validate())
		Update();
	
	gld_shape result;
	result.create(m_Shape, TMatrix(), TCxform(), NULL);

	return result;
}

gldInstance *CBackground::Detach()
{
	gldInstance *inst = m_pInst;
	m_pInst = NULL;
	return inst;
}

void CBackground::Attach(gldInstance *pInst)
{
	if (m_pInst == pInst)
		return;
	if (m_pInst != NULL)
		delete m_pInst;
	m_pInst = pInst;	
}

void CBackground::Update()
{
	gldMainMovie2 *movie = _GetMainMovie2();
	ASSERT(movie != NULL);
	int width = movie->m_width;
	int height = movie->m_height;
	gldObj *obj = GetObj();
	gld_shape bshape = CTraitShape(obj);
	
	DestroyShape();

	gld_rect bound = bshape.bound();

	m_Shape.create_special_shape();

	TMatrix mx;	
	TCxform cx;
	CTransAdaptor::GM2TM(GetMatrix(), mx);
	CTransAdaptor::GCX2TCX(m_pInst->m_cxform, cx);

	if (bound.width() == 0 || bound.height() == 0)
	{
		gld_shape subshape;
		subshape.create(bshape, mx, cx, NULL);
		m_Shape.push_back(subshape);
		return;
	}
	
	if (m_Pos == POS_CENTER)
	{			
		gld_shape subshape;
		subshape.create(bshape, mx, cx, NULL);
		m_Shape.push_back(subshape);
	}
	else if (m_Pos == POS_STRETCH)
	{		
		gld_shape subshape;
		subshape.create(bshape, mx, cx, NULL);
		m_Shape.push_back(subshape);
	}
	else
	{
		gld_shape subshape;
		subshape.create(bshape, mx, cx, NULL);
		m_Shape.push_back(subshape);
	}
	m_Shape.bound(m_Shape.calc_bound());
}

gldMatrix CBackground::GetMatrix()
{
	gldMainMovie2 *movie = _GetMainMovie2();
	ASSERT(movie != NULL);
	int width = movie->m_width;
	int height = movie->m_height;
	gldObj *obj = GetObj();
	gld_shape bshape = CTraitShape(obj);
	gldMatrix mx = m_pInst->m_matrix;	
	gld_rect bound = bshape.bound();	
	if (m_Pos == POS_CENTER)
	{
		int bx = (bound.left + bound.right) / 2;
		int by = (bound.top + bound.bottom) / 2;
		int offsetx = (int)(width * 20 / 2 - bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(height * 20 / 2 - bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}
	else if (m_Pos == POS_STRETCH)
	{
		int bx = (bound.left + bound.right) / 2;
		int by = (bound.top + bound.bottom) / 2;
		TMatrix imx;
		CTransAdaptor::GM2TM(m_pInst->m_matrix, imx);
		bound = _transform_rect(bound, imx);
		if (bound.width() == 0 || bound.height() == 0)
			return mx;
		float sx = (float)width * 20 / bound.width();
		float sy = (float)height * 20 / bound.height();
		gldMatrix smx(sx, sy, 0, 0, 0, 0);
		mx = mx * smx;
		int offsetx = (int)(width * 20 / 2 - bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(height * 20 / 2 - bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}
	else if (m_Pos == POS_TOPLEFT)
	{
		TMatrix imx;
		CTransAdaptor::GM2TM(m_pInst->m_matrix, imx);
		bound = _transform_rect(bound, imx);		
		int bx = bound.left;
		int by = bound.top;
		imx = imx.Invert();
		imx.Transform(bx, by);
		int offsetx = (int)(- bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(- bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}
	else if (m_Pos == POS_TOPRIGHT)
	{
		TMatrix imx;
		CTransAdaptor::GM2TM(m_pInst->m_matrix, imx);
		bound = _transform_rect(bound, imx);		
		int bx = bound.right;
		int by = bound.top;
		imx = imx.Invert();
		imx.Transform(bx, by);
		int offsetx = (int)(width * 20 - bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(- bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}
	else if (m_Pos == POS_BOTTOMLEFT)
	{
		TMatrix imx;
		CTransAdaptor::GM2TM(m_pInst->m_matrix, imx);
		bound = _transform_rect(bound, imx);		
		int bx = bound.left;
		int by = bound.bottom;
		imx = imx.Invert();
		imx.Transform(bx, by);
		int offsetx = (int)(- bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(height * 20 - bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}
	else if (m_Pos == POS_BOTTOMRIGHT)
	{
		TMatrix imx;
		CTransAdaptor::GM2TM(m_pInst->m_matrix, imx);
		bound = _transform_rect(bound, imx);		
		int bx = bound.right;
		int by = bound.bottom;
		imx = imx.Invert();
		imx.Transform(bx, by);
		int offsetx = (int)(width * 20 - bx * mx.m_e11 - by * mx.m_e21);
		int offsety = (int)(height * 20 - bx * mx.m_e12 - by * mx.m_e22);
		mx.m_x = offsetx;
		mx.m_y = offsety;
	}	

	return mx;
}

void CBackground::DestroyShape()
{
	if (m_Shape.validate())			
		m_Shape.destroy(true, false);	
}

BOOL CBackground::BuildEffects(gldInstance::EnumErrorHandleMethod method /*= IS_PROMPT*/)
{
	if (m_pInst->m_effectList.size() == 0)
		return TRUE;

	gldMatrix mx = m_pInst->m_matrix;
	gldCxform cx = m_pInst->m_cxform;

	m_pInst->m_matrix = gldMatrix();
	m_pInst->m_cxform = gldCxform();

	BOOL ret = m_pInst->BuildEffects(method);

	m_pInst->m_matrix = mx;
	m_pInst->m_cxform = cx;

	return ret;
}

HRESULT CBackground::ReadFromStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	WORD ver = 1;
	HRESULT hr = stm.Read(ver);
	CHECKRESULT();
	if (ver == 1)
	{
		hr = stm.Read(m_Pos);
		CHECKRESULT();
		hr = m_pInst->ReadFromStream(pStm, pUnk);
		CHECKRESULT();
	}
	else
		hr = E_FAIL;
	return hr;
}

HRESULT CBackground::WriteToStream(IStream *pStm, void *pUnk)
{
	IStreamWrapper stm(pStm);
	WORD ver = 1;
	HRESULT hr = stm.Write(ver);
	CHECKRESULT();
	hr = stm.Write(m_Pos);
	CHECKRESULT();
	hr = m_pInst->WriteToStream(pStm, pUnk);
	CHECKRESULT();
	return S_OK;
}

CBackground *CBackground::FromStream(IStream *pStm, void *pUnk)
{
	CBackground *bk = new CBackground;
	HRESULT hr = bk->ReadFromStream(pStm, pUnk);
	if (FAILED(hr))
	{
		delete bk;
		bk = NULL;
	}
	return bk;
}






/* CCmdChangeSceneBackgrnd */

CCmdChangeSceneBackgrnd::CCmdChangeSceneBackgrnd(gldScene2 *scene, gldObj *obj, int pos, const gldMatrix &mx, const gldCxform &cx)
: m_pScene(scene)
, m_bAttached(false)
, m_pInst(NULL)
{
	_M_Desc_ID = IDS_CMD_CHANGESCENEBACKGRND;
	ASSERT(obj != NULL);
	m_pBackgrnd = new CBackground(obj, pos, mx, cx);
}

CCmdChangeSceneBackgrnd::CCmdChangeSceneBackgrnd(gldScene2 *scene, gldInstance *pInst, int pos, bool bAttached)
: m_pScene(scene)
, m_bAttached(bAttached)
, m_pInst(pInst)
{
	_M_Desc_ID = IDS_CMD_CHANGESCENEBACKGRND;
	ASSERT(pInst != NULL);
	m_pBackgrnd = new CBackground((gldInstance *)0, pos);
}

CCmdChangeSceneBackgrnd::CCmdChangeSceneBackgrnd(gldScene2 *scene)
: m_pScene(scene)
, m_bAttached(false)
, m_pBackgrnd(NULL)
, m_pInst(NULL)
{
	_M_Desc_ID = IDS_CMD_CHANGESCENEBACKGRND;
}

CCmdChangeSceneBackgrnd::~CCmdChangeSceneBackgrnd()
{
	if (m_pBackgrnd != NULL)
		delete m_pBackgrnd;
}

bool CCmdChangeSceneBackgrnd::Execute()
{
	if (m_bAttached)
	{
		if (m_pBackgrnd != NULL)
			m_pBackgrnd->Attach(m_pInst);		
	}
	swap(m_pBackgrnd, m_pScene->m_backgrnd);

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Execute();
}

bool CCmdChangeSceneBackgrnd::Unexecute()
{
	if (m_bAttached)
	{
		if (m_pScene->m_backgrnd != NULL)
			m_pInst = m_pScene->m_backgrnd->Detach();		
	}
	swap(m_pBackgrnd, m_pScene->m_backgrnd);

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();

	return TCommand::Unexecute();
}


/* CCmdChangeBackgrndPos */

CCmdChangeBackgrndPos::CCmdChangeBackgrndPos(CBackground *backgrnd, int pos)
: m_Backgrnd(backgrnd)
, m_Pos(pos)
{
	_M_Desc_ID = IDS_CMD_CHANGESCENEBACKGRND;
}

bool CCmdChangeBackgrndPos::Execute()
{
	Do();

	return TCommand::Execute();
}

bool CCmdChangeBackgrndPos::Unexecute()
{
	Do();

	return TCommand::Unexecute();
}

void CCmdChangeBackgrndPos::Do()
{
	int pos = m_Backgrnd->GetPosition();	
	m_Backgrnd->SetPosition(m_Pos);	
	m_Pos = pos;	

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();
}
