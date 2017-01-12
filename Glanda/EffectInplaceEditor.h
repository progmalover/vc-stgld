#pragma once
#include "Observer.h"
#include "Singleton.h"
#include "Glanda_i.h"
#include "Tool.h"
#include "Matrix.h"
#include "gld_draft_dc.h"
#include "Command.h"

class gldEffect;
class gldInstance;

class CCmdTransformPath : public TCommand
{
public:
	CCmdTransformPath(gldEffect *pEffect, TMatrix &newMatrix);

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	gldEffect *m_pEffect;
	TMatrix m_Matrix;
};

class CCmdRelinePath : public TCommand
{
public:
	CCmdRelinePath(IGLD_CurvedPath *pIPath, double offsetX, double offsetY);

	virtual bool Execute();
	virtual bool Unexecute();

protected:
	CComPtr<IGLD_CurvedPath> m_pIPath;
	double m_OffsetX;
	double m_OffsetY;
};

class CResizePathTool : public TEvent
{
public:
	CResizePathTool();
	virtual ~CResizePathTool();

	BOOL Start(gldInstance *pInst, gldEffect *pEffect, IGLD_CurvedPath *pIPath, int nHandler, const CPoint &point);

protected:
	virtual POINT GetPoint(const POINT &p);	
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);	

	void PrepareDC();
	void UnprepareDC();

	void DrawEffect();
	void RecalcPathMatrix(UINT nFlags);
	void Do(gldEffect *pEffect, IGLD_CurvedPath *pIPath);

protected:
	HDC	m_hDC;
	HPEN m_hOldPen;
	HPEN m_hEffectPen;	
	int m_nHandler;	
	SAFEARRAY *m_pArrData;
	double m_bx;
	double m_by;
	ULONG m_nArrData;
	TMatrix m_PathMatrix;
	TMatrix m_BasePathMatrix;
	double m_cx;
	double m_cy;
	double m_sx;
	double m_sy;
	double m_ex;
	double m_ey;
	int m_nOldR2;
	BOOL m_bDo;
};

class CEffectInplaceEditor:public CCmdTarget
{
public:
	CEffectInplaceEditor(void);
	virtual ~CEffectInplaceEditor(void);

	void DrawEffect(HDC hDC);	
	void DrawPath(HDC hDC, IGLD_CurvedPath *pIPath, IGLD_Matrix *pIMx);
	void DrawHandler(HDC hDC, IGLD_Rect *pIRect, IGLD_Matrix *pIMx);
	void GetHandler(IGLD_Rect *pIRect, IGLD_Matrix *pIMx, POINT *arrHandler);
	bool PtOnPath(const CPoint &point);
	int HitTest(const CPoint &point);
	void Track(const CPoint &point);
	BOOL SetCursor(const CPoint &point);

	DECLARE_GLANDA_OBSERVER(CEffectInplaceEditor, ChangeTimelineSel)

protected:
	CResizePathTool	m_PathTool;
};


typedef CSingleton<CEffectInplaceEditor> EffectInplaceEditor;