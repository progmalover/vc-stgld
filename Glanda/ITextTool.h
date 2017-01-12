#pragma once

#include "gldFont.h"
#include "gldText.h"
#include "gldColor.h"
#include "gldLibrary.h"
#include "gldTextGlyphRecord.h"
#include "TextToolEx.h"
#include "my_app.h"
#include "CmdAddShape.h"
#include "CmdChangeText.h"

class ITextTool
{
public:
	ITextTool(void);
	virtual ~ITextTool(void);

public:
	// Interface need by TTextToolExTmpl class
	bool CanAddText()
	{
		return my_app.CanAddShape();
	}

	TGraphicsDevice *GetGraphDev()
	{
		return my_app.GetGraphicsDevice();
	}

	gld_layer GetCurLayer()
	{
		return my_app.GetCurLayer();
	}

	void Repaint()
	{
		my_app.Redraw();
		my_app.Repaint();
	}
	
	void Redraw()
	{
		my_app.Redraw();
	}

	void Redraw(const gld_rect &rc)
	{
		my_app.Redraw(rc);
	}

	gld_shape PtOnShape(int x, int y)
	{
		return my_app.PtOnShape(x, y);
	}

	HCURSOR StockCursor(int nIndex)
	{
		return my_app.GetStockCursor(nIndex);
	}

	void AddText(gldText2 *pText)
	{
		my_app.Commands().Do(new CCmdAddText(pText));		

	}

	void LeaveEdit()
	{
		if (!my_app.LockTool())
			my_app.SetCurTool(IDT_TRANSFORM);
	}

	void ModifyText(gldText2 *pOldText, gldText2 *pText)
	{
		my_app.Commands().Do(new CCmdChangeText(pOldText, pText));		
	}

	void DoSelect(gld_shape_sel &sel)
	{
		my_app.DoSelect(sel, false);
	}

	gld_shape_sel &CurSel()
	{
		return my_app.CurSel();
	}

	gld_shape PtOnText(int x, int y)
	{
		gld_frame frame = my_app.CurFrame();

		gld_layer_iter li = frame.rbegin_layer();

		for (; li != frame.rend_layer(); --li)
		{
			if ((*li).attributes() == LA_NORMAL)
			{
				gld_shape_iter si = (*li).rbegin_shape();
				for(; si != (*li).rend_shape(); --si) 
				{
					gld_shape gs = *si;
					if (gs.ptr() != NULL)
					{					
						gldObj *pObj = CTraitInstance(gs);

						if (pObj->IsGObjInstanceOf(gobjText))
						{
							int xx = x, yy = y;

							gs.matrix().Invert().Transform(xx, yy);
							gld_rect box = gs.bound();

							if (box.pt_in_rect(xx, yy))
							{
								return gs;
							}
						}
					}
				}
			}
		}

		return gld_shape();
	}

	CSize GetViewSize()
	{
		return my_app.GetViewSize();
	}

	int GetHScrollPos()
	{
		return my_app.GetHScrollPos();
	}

	int GetVScrollPos()
	{
		return my_app.GetVScrollPos();
	}

	void GetHorzOffsetRange(double &xmin, double &xmax)
	{
		my_app.GetHorzOffsetRange(xmin, xmax);
	}

	void GetVertOffsetRange(double &ymin, double &ymax)
	{
		my_app.GetVertOffsetRange(ymin, ymax);
	}

	void Scroll(int dx, int dy)
	{
		my_app.Scroll(dx, dy);
	}	
};
