#include "stdafx.h"
#include "Options.h"
#include "GlandaDoc.h"
#include "filepath.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SINGLETON(COptions)
COptions::COptions()
{
	CWinApp *pApp = AfxGetApp();

	m_bReloadLastDocument = pApp->GetProfileInt("Options", "Reload Last Document", FALSE);
	m_bShowContextSensitiveTips = pApp->GetProfileInt("Options", "Show Context Sensitive Tips", TRUE);
	m_bCacheSWFResources = pApp->GetProfileInt("Options", "Cache SWF Resources", TRUE);

	m_bShowCanvasBorder = pApp->GetProfileInt("Options", "Show Canvas Border", TRUE);
	m_bShowDesignToolTip = pApp->GetProfileInt("Options", "Show Design Window Tips", TRUE);
	m_bCenterDesignToolTip = pApp->GetProfileInt("Options", "Center Design Window Tips", TRUE);

	m_bShowTimelineObjName = pApp->GetProfileInt("Options", "Show Timeline Object Name", TRUE);
	m_bShowTimelineToolTip = pApp->GetProfileInt("Options", "Show Timeline Tips", TRUE);

	m_bAutoListMembers = pApp->GetProfileInt("Options", "Auto Lise Members", TRUE);
	m_bAutoShowParameterInfo = pApp->GetProfileInt("Options", "Auto Show Parameter Info", TRUE);
	m_bAutoIndent = pApp->GetProfileInt("Options", "Auto Indent", TRUE);
	m_bShowSelMargin = pApp->GetProfileInt("Options", "Show Sel Margin", TRUE);
	m_bShowLineNumbers = pApp->GetProfileInt("Options", "Show Line Numbers", TRUE);
	

	CString strPreviewDirOrder = AfxGetApp()->GetProfileString("Options\\Preview", "Order", "0 1 2");
	BOOL bError = FALSE;
	if (sscanf(strPreviewDirOrder, "%d %d %d", &m_nPreviewDir[0], &m_nPreviewDir[1], &m_nPreviewDir[2]) == 3)
	{
		for (int i = 0; !bError && i < 3; i++)
		{
			if (m_nPreviewDir[i] < 0 || m_nPreviewDir[i] > 2)
			{
				bError = TRUE;
				break;
			}

			for (int j = 0; j < i - 1; j++)
			{
				if (m_nPreviewDir[j] == m_nPreviewDir[i])
				{
					bError = TRUE;
					break;
				}
			}
		}
	}
	else
	{
		bError = TRUE;
	}

	if (bError)
		for (int i = 0; i < 3; i++)
			m_nPreviewDir[i] = i;

	m_bQuitFullscreenOnEsc = pApp->GetProfileInt("Options", "Quit Fullscreen On Esc", TRUE);
	m_bClosePreviewOnEsc = pApp->GetProfileInt("Options", "Close Preview On Esc", TRUE);
	m_bAutoHideCaption = pApp->GetProfileInt("Options", "Auto Hide Caption", FALSE);

	m_bShowAllElementsInTimeline = pApp->GetProfileInt("Options", "Show All Elements in Timeline", FALSE);
}

COptions::~COptions()
{
	CWinApp *pApp = AfxGetApp();

	pApp->WriteProfileInt("Options", "Reload Last Document", m_bReloadLastDocument);
	pApp->WriteProfileInt("Options", "Show Context Sensitive Tips", m_bShowContextSensitiveTips);
	pApp->WriteProfileInt("Options", "Cache SWF Resources", m_bCacheSWFResources);

	pApp->WriteProfileInt("Options", "Show Canvas Border", m_bShowCanvasBorder);
	pApp->WriteProfileInt("Options", "Show Design Window Tips", m_bShowDesignToolTip);
	pApp->WriteProfileInt("Options", "Center Design Window Tips", m_bCenterDesignToolTip);

	pApp->WriteProfileInt("Options", "Show Timeline Object Name", m_bShowTimelineObjName);
	pApp->WriteProfileInt("Options", "Show Timeline Tips", m_bShowTimelineToolTip);

	pApp->WriteProfileInt("Options", "Auto Lise Members", m_bAutoListMembers);
	pApp->WriteProfileInt("Options", "Auto Show Parameter Info", m_bAutoShowParameterInfo);
	pApp->WriteProfileInt("Options", "Auto Indent", m_bAutoIndent);
	pApp->WriteProfileInt("Options", "Show Sel Margin", m_bShowSelMargin);
	pApp->WriteProfileInt("Options", "Show Line Numbers", m_bShowLineNumbers);

	CString strPreviewDirOrder;
	strPreviewDirOrder.Format("%d %d %d", m_nPreviewDir[0], m_nPreviewDir[1], m_nPreviewDir[2]);
	AfxGetApp()->WriteProfileString("Options\\Preview", "Order", strPreviewDirOrder);

	pApp->WriteProfileInt("Options", "Quit Fullscreen On Esc", m_bQuitFullscreenOnEsc);
	pApp->WriteProfileInt("Options", "Close Preview On Esc", m_bClosePreviewOnEsc);
	pApp->WriteProfileInt("Options", "Auto Hide Caption", m_bAutoHideCaption);

	pApp->WriteProfileInt("Options", "Show All Elements in Timeline", m_bShowAllElementsInTimeline);
}

/*
0 - Last export directory
1 - Document (*.sqf) save directory
2 - System temporary directory
*/
CString COptions::GetPreviewPath()
{
	CString strPath;
	CGlandaDoc *pDoc = _GetCurDocument();
	for (int i= 0; i < 3; i++)
	{
		switch (m_nPreviewDir[i])
		{
		case 0:
			if (!pDoc->m_strMovieExport.IsEmpty())
			{
				strPath = pDoc->m_strMovieExport;
				::PathRemoveFileSpec(strPath.GetBuffer(0));
				strPath.ReleaseBuffer();
				if (IsDirWritable(strPath))
					return strPath;
			}
			break;

		case 1:
			if (!pDoc->GetPathName().IsEmpty())
			{
				strPath = pDoc->GetPathName();
				::PathRemoveFileSpec(strPath.GetBuffer(0));
				strPath.ReleaseBuffer();
				if (IsDirWritable(strPath))
					return strPath;
			}
			break;

		case 2:
			if (GetTempPath(_MAX_PATH, strPath.GetBuffer(_MAX_PATH + 1)) != 0)
			{
				strPath.ReleaseBuffer();
				return strPath;
			}
			break;
		}
	}
	strPath = ::GetModuleFilePath();
	return strPath;
}
