// TargetTree.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "TargetTree.h"

#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldCharacter.h"
#include "gldObj.h"
#include "gldSprite.h"

#include "Symbols.h"

// CTargetTree

CTargetTree::CTargetTree()
{
	m_bShowNamedInstancesOnly = FALSE;
	m_hItemCurrentMovieClip = NULL;
}

CTargetTree::~CTargetTree()
{
}


BEGIN_MESSAGE_MAP(CTargetTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()



// CTargetTree message handlers

void CTargetTree::PopulateTree(HTREEITEM hParentItem, gldCharacterKey *pCK)
{
	if (hParentItem == TVI_ROOT)
	{
		m_hItemCurrentMovieClip = NULL;
		DeleteAllItems();
	}

	gldMovieClip *pMC;
	int nImage;
	if (pCK)
	{
		gldObj *pObj = pCK->GetObj();
		GObjType type = pObj->GetGObjType();
		switch (type)
		{
		case gobjSprite:
			pMC = ((gldSprite *)pObj)->m_mc;
			nImage = INDEX_SPRITE;
			break;

		case gobjButton:
			pMC = ((gldButton *)pObj)->m_bmc;
			nImage = INDEX_BUTTON;
			break;
		default:
			return;
		}
	}
	else
	{
		pMC = gldDataKeeper::Instance()->GetCurMovieClip();
		nImage = INDEX_MOVIE;
	}

	CString strName;
	if (pCK != NULL)
	{
		if (pCK->m_name.empty())
		{
			if (m_bShowNamedInstancesOnly)
				return;
			strName.Format("(%s)", pCK->GetObj()->m_name.c_str());
		}
		else
		{
			strName = pCK->m_name.c_str();
		}
	}
	else
	{
		strName = "_root";
	}

	hParentItem = InsertItem(strName, nImage, nImage, hParentItem);
	SetItemData(hParentItem, (DWORD_PTR)pCK);
	if (pMC == _GetCurMovieClip())
		m_hItemCurrentMovieClip = hParentItem;

	if (nImage == INDEX_BUTTON)	// Flash specification
		return;

	for (GLAYER_LIST_IT it = pMC->m_layerList.begin(); it != pMC->m_layerList.end(); it++)
	{
		gldLayer *pLayer = *it;

		for (GFRAMECLIP_LIST_IT it_fc = pLayer->m_frameClipList.begin(); it_fc != pLayer->m_frameClipList.end(); it_fc++)
		{
			gldFrameClip *pFC = *it_fc;

			for (GCHARACTERKEY_LIST_IT it_ck = pFC->m_characterKeyList.begin(); it_ck != pFC->m_characterKeyList.end(); it_ck++)
			{
				gldCharacterKey *pCK = *it_ck;
				PopulateTree(hParentItem, pCK);
			}
		}
	}

	if (ItemHasChildren(hParentItem))
		Expand(hParentItem, TVE_EXPAND);
}

void CTargetTree::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		if (bmp.LoadBitmap(IDB_SYMBOLS))
		{
			m_imgList.Add(&bmp, RGB(255, 0, 255));
			SetImageList(&m_imgList, TVSIL_NORMAL);
		}
	}

	CTreeCtrl::PreSubclassWindow();
}

void CTargetTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVCUSTOMDRAW *pcd = (NMTVCUSTOMDRAW *)pNMHDR;

	// TODO: Add your control notification handler code here

	*pResult = CDRF_DODEFAULT;

	switch (pcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}

		case CDDS_ITEMPREPAINT:
		{
			gldCharacterKey *pCK = (gldCharacterKey *)pcd->nmcd.lItemlParam;
			if ((pcd->nmcd.uItemState & CDIS_SELECTED) == 0)
				if (pCK && pCK->m_name.empty())
					pcd->clrText = ::GetSysColor(COLOR_GRAYTEXT);
			break;
		}
	}
}
