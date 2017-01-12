#pragma once
#include "FlexListCtrl.h"
#include "TextStyleManager.h"


class CDib;

// CTextStyleListCtrl
class CTextStyleListCtrl : public CFlexListCtrl
{
	//DECLARE_DYNAMIC(CTextStyleListCtrl)

	struct _ItemData
	{
		CTextStyleData* m_pTextStyle;
		CAutoPtr<CDib>			m_pBitmapBuffer;

		_ItemData(CTextStyleData* pTextStyle)
			: m_pTextStyle(pTextStyle)
		{
		}
		~_ItemData()
		{
			delete m_pTextStyle;
		}
	};
	enum	{ SPACING = 2	};

public:
	CTextStyleListCtrl();
	virtual ~CTextStyleListCtrl();

	void InsertTextStyle(int index, CTextStyleData* pTextStyle);
	CTextStyleData* GetTextStyle(int index);

protected:
	DECLARE_MESSAGE_MAP()


	virtual void OnDeleteItem(int index);
	virtual void DrawItem(CDC* pDC, int index, const CRect* pRect);
	void RenderEx(CDC& dc, CRect& rc, CTextStyleData* pTextStyle);
};


