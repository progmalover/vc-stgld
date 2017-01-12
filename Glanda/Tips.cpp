#include "StdAfx.h"
#include "Glanda.h"

#include "tips.h"

#include "BalloonToolTip.h"
#include "DesignWnd.h"

#include "CheckMessageBox.h"
#include "CmdBreakApart.h"

#include "my_app.h"
#include "gldObj.h"

#include "DrawHelper.h"

#include "Options.h"
#include "SWFProxy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void ShowPenToolTip(CPoint point, BOOL bPenTool)
{
	if (!COptions::Instance()->m_bShowContextSensitiveTips)
		return;

	if (AfxGetApp()->GetProfileInt("Tips", "Pen", TRUE))
	{
		CString strTool;
		strTool.LoadString(bPenTool ? IDS_TOOL_PEN : IDS_TOOL_PEN_MOTION_PATH);
		CString strText, strTitle;
		strText.Format(IDS_TIP_PEN_TOOL1, (LPCTSTR)strTool);
		strTitle.Format(IDS_TIP_PEN_TOOL_TITLE1, (LPCTSTR)strTool);
	
		CDesignWnd::Instance()->ClientToScreen(&point);
		CBalloonToolTip::Instance()->Show(
			point.x + 5, 
			point.y - 5, 
			strText, 
			strTitle);

		AfxGetApp()->WriteProfileInt("Tips", "Pen", FALSE);
	}
}

void ShowGroupBeforeAddEffectTip()
{
	ASSERT (my_app.CurSel().count() > 1);

	if (!COptions::Instance()->m_bShowContextSensitiveTips)
		return;

	if (AfxGetApp()->GetProfileInt("Tips", "Group Before Add Effect", TRUE))
	{
		BOOL bChecked = FALSE;
		CheckMessageBox(IDS_TIP_GROUP_BEFORE_ADD_EFFECT, 
			IDS_DONOT_SHOW_AGAIN, 
			bChecked, 
			MB_ICONINFORMATION | MB_OK);

		AfxGetApp()->WriteProfileInt("Tips", "Group Before Add Effect", !bChecked);
	}
}

void ShowColorButtonTip(CPoint point)
{
	if (!COptions::Instance()->m_bShowContextSensitiveTips)
		return;

	if (AfxGetApp()->GetProfileInt("Tips", "Color Button", TRUE))
	{
		CString str;
		CBalloonToolTip::Instance()->Show(
			point.x, 
			point.y, 
			IDS_TIP_COLOR_BUTTON, 
			IDS_TIP_COLOR_BUTTON_TITLE);
		
		AfxGetApp()->WriteProfileInt("Tips", "Color Button", FALSE);
	}
}
