#pragma once

#include "gld_selection.h"
#include <list>

using namespace std;

class gldInstance;
class gldScene2;
class CChangeZorder
{
public:	
	list<int>	m_lstZorder;
	gldScene2	*m_pScene;

public:
	CChangeZorder(void);
	~CChangeZorder(void);

	void BuildFromSelection(const gld_shape_sel &sel);

private:
	void AddData(gldInstance *pInst);	
};
