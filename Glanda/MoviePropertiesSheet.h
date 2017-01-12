#pragma once

#include "MoviePropertiesPage.h"
#include "MovieSoundPage.h"

// CMoviePropertiesSheet

class CMoviePropertiesSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMoviePropertiesSheet)

public:
	CMoviePropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMoviePropertiesSheet();

protected:
	DECLARE_MESSAGE_MAP()

	CMoviePropertiesPage m_dlgProperties;
	CMovieSoundPage m_dlgSound;
	static int m_nActivePage;
public:
	virtual INT_PTR DoModal();
	afx_msg void OnDestroy();
};


