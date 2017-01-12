#pragma once

#include "Utils.h"

class COptions
{
private:
	COptions();
	~COptions();

public:
	DECLARE_SINGLETON(COptions)

public:
	BOOL m_bReloadLastDocument;
	BOOL m_bShowContextSensitiveTips;
	BOOL m_bCacheSWFResources;

	BOOL m_bShowCanvasBorder;
	BOOL m_bShowDesignToolTip;
	BOOL m_bCenterDesignToolTip;

	BOOL m_bShowTimelineObjName;
	BOOL m_bShowTimelineToolTip;

	BOOL m_bAutoListMembers;
	BOOL m_bAutoShowParameterInfo;
	BOOL m_bAutoIndent;
	BOOL m_bShowSelMargin;
	BOOL m_bShowLineNumbers;

	int m_nPreviewDir[3];
	BOOL m_bQuitFullscreenOnEsc;
	BOOL m_bClosePreviewOnEsc;
	BOOL m_bAutoHideCaption;

	BOOL m_bShowAllElementsInTimeline;
public:
	CString GetPreviewPath();
};
