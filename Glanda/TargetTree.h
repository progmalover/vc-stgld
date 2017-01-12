#pragma once

class gldCharacterKey;

// CTargetTree

class CTargetTree : public CTreeCtrl
{
public:
	CTargetTree();
	virtual ~CTargetTree();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void PopulateTree(HTREEITEM hParentItem, gldCharacterKey *pCK);
	HTREEITEM m_hItemCurrentMovieClip;
protected:
	virtual void PreSubclassWindow();
	CImageList m_imgList;
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

public:
	BOOL m_bShowNamedInstancesOnly;
};
