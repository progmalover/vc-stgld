#pragma once

#include "DlgTextTool.h"

// CDlgToolOptionTextTool dialog

class CDlgToolOptionTextTool : public CDlgTextTool
{
public:
	CDlgToolOptionTextTool(CWnd *pParentWnd = 0);
	virtual ~CDlgToolOptionTextTool();

	DECLARE_SINGLETON(CDlgToolOptionTextTool)

	// Dialog Data
	enum { IDD = IDD_TOOL_OPTION_TEXT_TOOL };
	virtual SETextToolEx* GetTextTool();
	virtual void OnTextChanged();

protected:
	DECLARE_MESSAGE_MAP()

private:
	DECLARE_GLANDA_OBSERVER(CDlgToolOptionTextTool, TextInplaceEdit)
	DECLARE_GLANDA_OBSERVER(CDlgToolOptionTextTool, TextSelChange)
};
