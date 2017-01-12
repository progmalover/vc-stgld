#pragma once

#include "EditEx.h"

// CSceneNameEdit

class CSceneNameEdit : public CEditEx
{
public:
	CSceneNameEdit();
	virtual ~CSceneNameEdit();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	int m_nMaxWidth;

public:
	void SetMaxWidth(int nMaxWidth);
	void RecalcWindowWidth();
};
