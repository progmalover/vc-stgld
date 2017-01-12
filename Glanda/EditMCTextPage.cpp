// EditMCTextPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCTextPage.h"
#include "EditMCSheet.h"
#include "gldObj.h"
#include "gldText.h"
#include "EditMCEditTextDialog.h"


// CEditMCTextPage dialog

IMPLEMENT_DYNAMIC(CEditMCTextPage, CPropertyPage)
CEditMCTextPage::CEditMCTextPage()
: CPropertyPage(CEditMCTextPage::IDD)
, m_pCmd(NULL)
{
	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Detach(&m_xCObserverModifyCurrentScene2);
}

CEditMCTextPage::~CEditMCTextPage()
{
}

void CEditMCTextPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT_PREVIEW, m_stcTextPreview);
}


BEGIN_MESSAGE_MAP(CEditMCTextPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_TEXT_LIST, OnLbnSelchangeTextList)
	ON_BN_CLICKED(IDC_BTN_EDIT_TEXT, OnBnClickedBtnEditText)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEditMCTextPage message handlers

BOOL CEditMCTextPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_TEXT_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();
	for(std::list<_TextData*>::iterator iter = pSheet->m_texts.begin(); iter != pSheet->m_texts.end(); ++iter)
	{
		_TextData* pData = *iter;
		int nIndex = pListBox->AddString(pData->GetListText());
		pListBox->SetItemData(nIndex, (DWORD_PTR)pData);
	}
	pListBox->SetCurSel(0);
	HandleSelChange();

	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Attach(&m_xCObserverModifyCurrentScene2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMCTextPage::HandleSelChange()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_TEXT_LIST);
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_TextData* pData = (_TextData*)data;

	gldText2* pText = (gldText2*)pData->obj;

	CTextFormat& textFormat = (*(pText->m_ParagraphList[0]->m_TextBlockList.begin()))->TextFormat;
	COLORREF clr = textFormat.GetFontColor();

	if(255-GetRValue(clr)<=10 && 255-GetGValue(clr)<=10 && 255-GetBValue(clr)<=10)
	{
		m_stcTextPreview.SetObj(pData->obj, RGB(0xA9, 0xA9, 0xA9));
	}
	else
	{
		m_stcTextPreview.SetObj(pData->obj);
	}
}

void CEditMCTextPage::HandleTextModified()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_TEXT_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();

	for(std::list<_TextData*>::iterator iter = pSheet->m_texts.begin(); iter != pSheet->m_texts.end(); ++iter)
	{
		// 可能OBJ被替换了,所以要更新
		_TextData* pData = *iter;
		pData->obj = (*pData->keys.begin())->GetObj();
	}
	HandleSelChange();
}

void CEditMCTextPage::OnLbnSelchangeTextList()
{
	HandleSelChange();
}

void CEditMCTextPage::OnBnClickedBtnEditText()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_TEXT_LIST);
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_TextData* pData = (_TextData*)data;

	CEditMCEditTextDialog dlg(pData->keys, (gldText2*)(pData->obj));
	if(dlg.DoModal()==IDOK)
	{
		if(dlg.m_pCmd)
		{
			CGuardDrawOnce xDrawOnce;
			CGuardSelKeeper xSelKeeper;
			if(m_pCmd == NULL)
			{
				m_pCmd = new TCommandGroup();
			}
			dlg.m_pCmd->Unexecute();
			m_pCmd->Do(dlg.m_pCmd);
		}
	}
	else
	{
		if(dlg.m_pCmd)
		{
			CGuardDrawOnce xDrawOnce;
			CGuardSelKeeper xSelKeeper;
			dlg.m_pCmd->Unexecute();
			delete dlg.m_pCmd;
			dlg.m_pCmd = NULL;
		}
	}
}

void CEditMCTextPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	CSubjectManager::Instance()->GetSubject("ModifyCurrentScene2")->Detach(&m_xCObserverModifyCurrentScene2);
}

IMPLEMENT_OBSERVER(CEditMCTextPage, ModifyCurrentScene2)
{
	HandleTextModified();
}