#include "StdAfx.h"
#include "drawhelper.h"
#include "gldObj.h"
#include "gldCharacter.h"
#include "my_app.h"

#include "Observer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGuardDrawOnce::CGuardDrawOnce()
{
	my_app.SetRedraw(false);
	my_app.SetRepaint(false);
}

CGuardDrawOnce::~CGuardDrawOnce()
{
	my_app.SetRedraw(true);
	my_app.SetRepaint(true);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CDrawHelper::CDrawHelper(void)
{
}

CDrawHelper::~CDrawHelper(void)
{
}

void CDrawHelper::UpdateObj(gldObj *pObj)
{
	CGuardDrawOnce	xDraw;

	gld_frame	   frame = my_app.CurFrame();
	gld_layer_iter iLayer = frame.begin_layer();

	for (; iLayer != frame.end_layer(); ++iLayer)
	{		
		for (gld_shape_iter iShape = (*iLayer).begin_shape(); iShape != (*iLayer).end_shape(); ++iShape)
		{
			UpdateCharacter(*iShape);
		}		
	}
}

void CDrawHelper::UpdateCharacter(gld_shape	&shape, bool notify)
{
	if (shape.validate())
	{
		my_app.Redraw(_transform_rect(shape.bound(), shape.matrix()));
		my_app.Repaint();

		if (notify)
			CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Notify(0);
	}
}
