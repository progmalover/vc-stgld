#include "StdAfx.h"
#include "groupupdate.h"
#include "GlandaDoc.h"
#include "my_app.h"
#include "global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BEGIN_UPDATE_GROUP()	\
	my_app.SetRepaint(false);	\
	my_app.SetRedraw(false);

#define END_UPDATE_GROUP()	\
	my_app.SetRedraw(true);	\
	my_app.SetRepaint(true);

CGroupUpdate::CGroupUpdate(void)
{
	BEGIN_UPDATE_GROUP()
}

CGroupUpdate::~CGroupUpdate(void)
{
	END_UPDATE_GROUP()
}
