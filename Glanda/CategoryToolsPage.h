#pragma once

#include "CategoryPage.h"
#include "toolbutton.h"
#include "my_app.h"

#include "Observer.h"
#include "Global.h"

#define IDT_LOCK	(IDT_LAST + 1)

const struct TOOL
{
	UINT nIDTool;
	UINT nIDBitmap;
	UINT nTip;
	bool bAddSep;
}
tools[] = 
{
	{IDT_TRANSFORM,		IDB_TOOL_SELECT,			IDS_TOOL_SELECT,				false},	
	{IDT_RESHAPE,		IDB_TOOL_RESHAPE,			IDS_TOOL_RESHAPE,				false},	
	{IDT_TRANSFORMFILL,	IDB_TOOL_FILL_TRANSFORM,	IDS_TOOL_FILL_TRANSFORM,		true},

	{IDT_LINE,			IDB_TOOL_LINE,				IDS_TOOL_LINE,					false}, 
	{IDT_PENCIL,		IDB_TOOL_PENCIL,			IDS_TOOL_PENCIL,				false}, 
	{IDT_OVAL,			IDB_TOOL_OVAL,				IDS_TOOL_OVAL,					false}, 
	{IDT_RECT,			IDB_TOOL_RECT,				IDS_TOOL_RECT,					false}, 
	{IDT_PEN,			IDB_TOOL_PEN,				IDS_TOOL_PEN,					false}, 
	{IDT_TEXTTOOLEX,	IDB_TOOL_TEXT,				IDS_TOOL_TEXT,					true}, 
	//{IDT_INKBOTTLE,		IDB_TOOL_INK_BOTTLE,		IDS_TOOL_INK_BOTTLE,			false}, 
	//{IDT_PAINTBUCKET,	IDB_TOOL_PAINT_BUCKET,		IDS_TOOL_PAINT_BUCKET,			false}, 
	//{IDT_PENCILGUIDLINE,IDB_TOOL_PENCIL_GUIDELINE,	IDS_TOOL_PENCIL_GUIDELINE,		true},

	{IDT_ZOOMOUT,		IDB_TOOL_ZOOMOUT,			IDS_TOOL_ZOOMIN,				false},
	{IDT_ZOOMIN,		IDB_TOOL_ZOOMIN,			IDS_TOOL_ZOOMOUT,				false},
	{IDT_HAND,			IDB_TOOL_HAND,				IDS_TOOL_HAND,					false},
	{IDT_LOCK,			IDB_TOOL_LOCK,				IDS_TOOL_LOCK,					true},
};

class CDlgToolOption;
typedef std::map <int, CDlgToolOption *> OPTION_DIALOG_LIST;

// CCategoryToolsPage dialog

class CCategoryToolsPage : public CCategoryPage
{
//	DECLARE_DYNAMIC(CCategoryToolsPage)

public:
	CCategoryToolsPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCategoryToolsPage();

// Dialog Data
	enum { IDD = IDD_CATEGORY_TOOLS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	BOOL m_bInit;
	CToolButton m_tools[sizeof(tools) / sizeof(TOOL)];
	CStatic m_seps[SEP_COUNT];

	CToolTipCtrl m_tooltip;

	afx_msg void OnTools(UINT nID);
	afx_msg void OnToolLock();
	int m_nCurTool;
	BOOL m_bLockCurrentTool;

	BOOL GetToolTipText(HWND hWnd, CString &strTip);
	BOOL OnToolTipText(UINT nID, NMHDR * pNMHDR, LRESULT * pResult);

public:
	int GetCurTool() {return m_nCurTool;}
	void RecalcLayout();
	void CreateTools();
	void CreateToolOptionDialogs();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();

	static void OnToolChanged(void *sender, void *receiver, int nNotifyID, LPARAM lParam);
	void ToolChanged(int nID);
	OPTION_DIALOG_LIST m_options;
	CDlgToolOption *GetOptionDialog(int nIDTool);

private :
	DECLARE_GLANDA_OBSERVER(CCategoryToolsPage, ChangeMainMovie2)
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
