#pragma once

#include "Glanda_i.h"
#include <atlsafe.h>
#include "Command.h"
#include "Matrix.h"

class CCurvedPath
{
public:
	CCurvedPath(IGLD_CurvedPath *pIPath = NULL);
	virtual ~CCurvedPath(void);
	
	void Draw(HDC hDC, TMatrix &mx);	

	CComPtr<IGLD_CurvedPath> m_Path;
};