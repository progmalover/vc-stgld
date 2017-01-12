#pragma once
#include "Observer.h"
#include "Global.h"
#include "Event.h"
#include "CmdUpdateObj.h"

// CRuleTool
class CRuleTool : public TEvent
{
public:
	CRuleTool();
	virtual ~CRuleTool();

	void SetDesignWnd(CWnd *pWnd);	

protected:
	void PrepareDC();
	void UnprepareDC();
	virtual POINT GetPoint(const POINT &p);	

	CDC		*m_pDC;
	CPen	m_DragPen;
	CPen	*m_OldPen;
	int		m_OldR2;
	CWnd	*m_DesignWnd;
};

// CVRuleTool
class CVRuleTool : public CRuleTool
{
public:
	CVRuleTool();

protected:
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);

	int	m_PrevX;
};

// CHRuleTool
class CHRuleTool : public CRuleTool
{
public:
	CHRuleTool();

protected:
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);

	int m_PrevY;
};

class CMoveRuleTool : public CRuleTool
{
public:
	CMoveRuleTool();

	void SetTarget(int pos, bool horz);

protected:
	virtual void OnBeginDrag(UINT nFlags, const POINT &point);
	virtual void OnDragOver(UINT nFlags, const POINT &point);
	virtual void OnCancelDrag(UINT nFlags, const POINT &point);
	virtual void OnDragDrop(UINT nFlags, const POINT &point);	
	virtual POINT GetPoint(const POINT &p);

	int		m_Pos;
	int		m_PrevPos;
	bool	m_Horz;
};

// CRuleWnd

class CRuleWnd : public CWnd
{
	DECLARE_DYNAMIC(CRuleWnd)
	
public:
	CRuleWnd();
	virtual ~CRuleWnd();

	DECLARE_SINGLETON(CRuleWnd)

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	void SetSibling(CWnd *sibling);
	BOOL ShowRule();
	void ShowRule(BOOL show);	

	void SetVernierPos(int h, int v, int hLength = 0, int vLength = 0);
	int GetRuleSize();

private:
	CWnd		*m_SiblingWnd;
	BOOL		m_ShowRule;
	int			m_RuleWidth;	
	CFont		m_HSmallFont;
	CFont		m_VSmallFont;
	CBitmap		m_HRuleBmp;
	CBitmap		m_VRuleBmp;	
	CVRuleTool	m_VRuleTool;
	CHRuleTool	m_HRuleTool;	
	
	int			m_HStart;
	int			m_HEnd;
	int			m_VStart;
	int			m_VEnd;

	int			m_LGradHeight;
	int			m_MGradHeight;
	int			m_SGradHeight;

	void DrawRule(CDC *pDC, const CRect &clip);
	void DrawHorzRule(CDC *pDC, int sttPix, int sttGrad, double pixPerGrad, int stepGrad, int maxGrad);
	void DrawVertRule(CDC *pDC, int sttPix, int sttGrad, double pixPerGrad, int stepGrad, int maxGrad);
	void DrawHorzVernier(CDC *pDC);
	void DrawVertVernier(CDC *pDC);
	void ClientToSibling(POINT &pt);
	void ClientToSibling(RECT &rc);
	void SiblingToClient(int &x, int &y);
	void SiblingToClient(POINT &pt);	

	DECLARE_GLANDA_OBSERVER(CRuleWnd, ChangeMapMode)
public:	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnBeginDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCancelDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragOver(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragDrop(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

// CCmdAddRule
class CCmdAddRule : public CCmdUpdateObjUniId
{
public:
	CCmdAddRule(int pos, bool horz);
	CCmdAddRule(int hPos, int vPos);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	int		m_HPos;
	bool	m_Horz;
	int		m_VPos;
	bool	m_Vert;
};

// CCmdRemoveRule
class CCmdRemoveRule : public CCmdUpdateObjUniId
{
public:
	CCmdRemoveRule(int pos, bool horz);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	int		m_Pos;
	bool	m_Horz;	
};

// CCmdMoveRule
class CCmdMoveRule : public CCmdUpdateObjUniId
{
public:
	CCmdMoveRule(int oldPos, int newPos, bool horz);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	int		m_OldPos;
	int		m_NewPos;
	bool	m_Horz;
	bool	m_Added;
};

