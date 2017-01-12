// EditGeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditGeneralPage.h"
#include "gldInstance.h"
#include <string>
#include "gldInstance.h"
#include "gldObj.h"
#include "gldSprite.h"
#include "EditMCSheet.h"
#include "GlandaCommand.h"
#include "InstanceActionSheet.h"

// CEditGeneralPage dialog

IMPLEMENT_DYNAMIC(CEditGeneralPage, CPropertyPage)
CEditGeneralPage::CEditGeneralPage(gldInstance* pInstance)
	: CPropertyPage(CEditGeneralPage::IDD, NULL)
	, m_pInstance(pInstance)
	, m_pCmd(NULL)
{
}

CEditGeneralPage::~CEditGeneralPage()
{
}

void CEditGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INSTANCE_NAME, m_editInstanceName);
}


BEGIN_MESSAGE_MAP(CEditGeneralPage, CPropertyPage)
	ON_CONTROL(EN_MODIFY, IDC_EDIT_INSTANCE_NAME, OnEditInstanceName)
	ON_BN_CLICKED(IDC_CHECK_MASK, OnBnClickedCheckMask)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_ACTION, OnBnClickedButtonEditAction)
END_MESSAGE_MAP()


// CEditGeneralPage message handlers

BOOL CEditGeneralPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	UINT nIcon;
	CString strTypeName;

	gldObj* pObj = m_pInstance->m_obj;
	switch(pObj->GetGObjType())
	{
	case gobjText:
		{
			nIcon = IDI_TEXT;
			strTypeName.LoadString(IDS_SYMBOL_TEXT);
			break;
		}

	case gobjShape:
		{
			nIcon = IDI_SHAPE;
			strTypeName.LoadString(IDS_SYMBOL_SHAPE);
			break;
		}

	case gobjSprite:
		{
			ASSERT(pObj);
			gldSprite *pSprite = (gldSprite *)pObj;
			if (pSprite->m_isGroup)
			{
				nIcon = IDI_GROUP;
				strTypeName.LoadString(IDS_SYMBOL_GROUP);
			}
			else
			{
				nIcon = IDI_MOVIECLIP;
				strTypeName.LoadString(IDS_SYMBOL_MOVIE_CLIP);
			}
			break;
		}

	case gobjButton:
		{
			nIcon = IDI_BUTTON;
			strTypeName.LoadString(IDS_SYMBOL_BUTTON);
			break;
		}

	default:
		ASSERT(FALSE);
	}

	((CStatic *)GetDlgItem(IDC_STATIC_TYPE_IMAGE))->SetIcon(AfxGetApp()->LoadIcon(nIcon));

	SetDlgItemText(IDC_STATIC_TYPE_NAME, strTypeName);

	SetDlgItemText(IDC_EDIT_INSTANCE_NAME, m_pInstance->m_name.c_str());

	if (m_pInstance->m_mask)
		CheckDlgButton(IDC_CHECK_MASK, BST_CHECKED);

	return TRUE;
}

void CEditGeneralPage::OnEditInstanceName()
{
	CString strName;
	GetDlgItemText(IDC_EDIT_INSTANCE_NAME, strName);
	strName.TrimLeft();
	strName.TrimRight();

	if(IsValidInstanceName(strName))
	{
		if(m_pCmd == NULL)
		{
			m_pCmd = new TCommandGroup();
		}
		m_pCmd->Do(new CCmdChangeInstanceName(m_pInstance, strName));
	}
}

void CEditGeneralPage::OnBnClickedCheckMask()
{
	// TODO: Add your control notification handler code here

	BOOL bMask = (IsDlgButtonChecked(IDC_CHECK_MASK) == BST_CHECKED);
	
	if(m_pCmd == NULL)
		m_pCmd = new TCommandGroup();

	m_pCmd->Do(new CCmdChangeInstanceMask(m_pInstance, bMask));
}

BOOL CEditGeneralPage::OnApply()
{
	CString strName;
	GetDlgItemText(IDC_EDIT_INSTANCE_NAME, strName);
	strName.TrimLeft();
	strName.TrimRight();

	if(IsValidInstanceName(strName))
	{
		return CPropertyPage::OnApply();
	}

	AfxMessageBoxEx(MB_OK|MB_ICONERROR, IDS_E_INVALID_INSTANCE_NAME, (LPCTSTR)strName);
	GotoDlgCtrl(GetDlgItem(IDC_EDIT_INSTANCE_NAME));

	return FALSE;
}

BOOL CEditGeneralPage::IsValidInstanceName(const CString& strName)
{
	for(int i = 0, j = strName.GetLength(); i < j; ++i)
	{
		TCHAR c = strName[i];
		if(!(c>=_T('a')&&c<=_T('z')
			|| c>=_T('A')&&c<=_T('Z')
			|| c==_T('_')
			|| i&&c>=_T('0')&&c<=_T('9')))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CEditGeneralPage::OnBnClickedButtonEditAction()
{
	// TODO: Add your control notification handler code here

	CString strCaption;
	strCaption.LoadString(IDS_KEY_ACTION);
	CInstanceActionSheet dlg(strCaption);
	dlg.DoModal();
}
