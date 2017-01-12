// EditMCSoundPage.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCSoundPage.h"
#include "EditMCSheet.h"
#include "gldObj.h"
#include "CmdReplaceSound.h"
#include "FileDialogEx.h"
#include "gldSound.h"
#include "CmdAddShape.h"
#include "EditMCReplaceDialog.h"
#include "ResourceExplorerWnd.h"
#include "filepath.h"
#include "gldSound.h"
#include "gldSWFEngine.h"
#include "Global.h"
#include "GlandaDoc.h"
#include "PlaySoundDialog.h"
#include ".\editmcsoundpage.h"
#include "gldFrameClip.h"
#include "gldLayer.h"


// CEditMCSoundPage dialog

IMPLEMENT_DYNAMIC(CEditMCSoundPage, CPropertyPage)
CEditMCSoundPage::CEditMCSoundPage()
: CPropertyPage(CEditMCSoundPage::IDD)
, m_pCmd(NULL)
{
}

CEditMCSoundPage::~CEditMCSoundPage()
{
}

void CEditMCSoundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditMCSoundPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_SOUND_LIST, OnLbnSelchangeSoundList)
	ON_BN_CLICKED(IDC_BTN_REPLACE_SOUND, OnBnClickedBtnReplaceSound)
	ON_BN_CLICKED(IDC_BTN_PLAY_SOUND, OnBnClickedBtnPlaySound)
	ON_BN_CLICKED(IDC_BTN_REMOVE_SOUND, OnBnClickedBtnRemoveSound)
END_MESSAGE_MAP()

// CEditMCSoundPage message handlers

BOOL CEditMCSoundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();
	for(std::list<_SoundData*>::iterator iter = pSheet->m_sounds.begin(); iter != pSheet->m_sounds.end(); ++iter)
	{
		_SoundData* pData = *iter;
		int nIndex = pListBox->AddString(pData->GetListText());
		pListBox->SetItemData(nIndex, (DWORD_PTR)pData);
	}
	pListBox->SetCurSel(0);
	HandleSelChange();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditMCSoundPage::HandleSelChange()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	if (pListBox->GetCurSel() > -1)
	{
		GetDlgItem(IDC_BTN_PLAY_SOUND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_REPLACE_SOUND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_REMOVE_SOUND)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BTN_PLAY_SOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REPLACE_SOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REMOVE_SOUND)->EnableWindow(FALSE);
	}
}

void CEditMCSoundPage::HandleSoundModified(bool bRemove /*= false*/)
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();
	for(std::list<_SoundData*>::iterator iter = pSheet->m_sounds.begin(); iter != pSheet->m_sounds.end(); ++iter)
	{
		// 可能OBJ被替换了,所以要更新
		_SoundData* pData = *iter;
		pData->obj = (*pData->keys.begin())->m_soundObj;
	}
	int nCurSel = pListBox->GetCurSel();
	if (bRemove)
	{
		pListBox->DeleteString(nCurSel);
		if (pListBox->GetCount() == 0)
			pListBox->SetCurSel(-1);
		else
		{
			if (nCurSel == pListBox->GetCount())
				nCurSel--;
			pListBox->SetCurSel(nCurSel);
		}
	}
	else
	{	
		DWORD_PTR data = pListBox->GetItemData(nCurSel);
		_SoundData* pData = (_SoundData*)data;
		pListBox->DeleteString(nCurSel);
		pListBox->InsertString(nCurSel, pData->GetListText());
		pListBox->SetItemData(nCurSel, data);
		pListBox->SetCurSel(nCurSel);
	}

	HandleSelChange();
}

void CEditMCSoundPage::OnLbnSelchangeSoundList()
{
	HandleSelChange();
}

void CEditMCSoundPage::OnBnClickedBtnReplaceSound()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	if (pListBox->GetCurSel() < 0)
		return;
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_SoundData* pData =(_SoundData*)data;
	if (pData == NULL || data == (DWORD_PTR)-1)
		return;
	CEditMCReplaceDialog dlg(RESOURCE_SOUNDS);
	if (dlg.DoModal() == IDOK && dlg.GetCurSelResource())
	{
		CResourceThumbItem* pThumbItem = dlg.GetCurSelResource();
		gldSound* pSound = NULL;
		if(pThumbItem->m_bStockResource)
		{
			if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjSound))
			{
				pSound = (gldSound*)(pThumbItem->m_pObj);
			}
			else
			{
				// Error
				return;
			}
		}
		else
		{
			pSound = new gldSound();
			if (!pSound->ReadFile(pThumbItem->m_strPathName))
			{
				delete pSound;
				AfxMessageBox(IDS_ERROR_IMPORT, MB_OK|MB_ICONERROR);
				return;
			}
			//pSound->UpdateUniId();
			_GetObjectMap()->UpdateUniId(pSound);
			pSound->m_name = FileStripPath(pThumbItem->m_strPathName);
		}

		TCommandGroup* pCmd = new TCommandGroup();
		pCmd->Do(new CCmdAddObj(pSound));
		for(std::list<gldSoundInstance*>::iterator iter = pData->keys.begin(); iter != pData->keys.end(); ++iter)
		{
			pCmd->Do(new CCmdReplaceSound(pSound, *iter));
		}

		if(!m_pCmd)
		{
			m_pCmd = new TCommandGroup();
		}
		m_pCmd->Do(pCmd);

		HandleSoundModified();
	}
}

void CEditMCSoundPage::OnBnClickedBtnPlaySound()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	if (pListBox->GetCurSel() < 0)
		return;
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_SoundData* pData =(_SoundData*)data;
	if (pData != NULL && data != (DWORD_PTR)-1)
	{
		gldSound* pSound = (gldSound*)pData->obj;

		CPlaySoundDialog dlg(pSound);
		dlg.DoModal();
	}
}

class CCmdRemoveFrameSound : public TCommand
{
public:
	CCmdRemoveFrameSound(gldLayer *layer, int t)
		: TCommand(IDS_CMD_REMOVEFRAMESOUND)
		, m_Layer(layer)
		, m_Time(t)
		, m_Sound(NULL)
	{
		gldFrameClip *frame = m_Layer->GetFrameClip((U16)m_Time);
		if (frame != NULL)
		{
			m_Sound = frame->m_sound->m_soundObj;
			m_SoundType = frame->m_sound->m_soundType;
		}
	}
	virtual ~CCmdRemoveFrameSound()
	{		
	}
	virtual bool Execute()
	{
		gldFrameClip *frame = m_Layer->GetFrameClip((U16)m_Time);
		if (frame != NULL)
		{
			frame->m_sound->m_soundType = soundNull;
			frame->m_sound->m_soundObj = NULL;
		}
		return TCommand::Execute();
	}
	virtual bool Unexecute()
	{
		gldFrameClip *frame = m_Layer->GetFrameClip((U16)m_Time);
		if (frame != NULL)
		{
			frame->m_sound->m_soundType = m_SoundType;
			frame->m_sound->m_soundObj = m_Sound;
		}
		return TCommand::Unexecute();
	}
private:	
	gldLayer *m_Layer;
	int m_Time;
	gldSound *m_Sound;
	int m_SoundType;
};

void CEditMCSoundPage::OnBnClickedBtnRemoveSound()
{
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_SOUND_LIST);
	if (pListBox->GetCurSel() < 0)
		return;
	DWORD_PTR data = pListBox->GetItemData(pListBox->GetCurSel());
	_SoundData* pData =(_SoundData*)data;
	if (pData != NULL && data != (DWORD_PTR)-1)
	{
		TCommandGroup* pCmd = new TCommandGroup();
		for (std::set<_MCFrame>::iterator i = pData->frames.begin(); i != pData->frames.end(); ++i)
		{
			const _MCFrame &frame = *i;
			pCmd->Do(new CCmdRemoveFrameSound(frame.layer, frame.ntime));
		}
		if(!m_pCmd)
		{
			m_pCmd = new TCommandGroup();
		}
		m_pCmd->Do(pCmd);
		CEditMCSheet* pSheet = (CEditMCSheet*)GetParent();
		std::list<_SoundData*>::iterator pos = find(pSheet->m_sounds.begin(), pSheet->m_sounds.end(), pData);
		ASSERT(pos != pSheet->m_sounds.end());
		delete *pos;
		pSheet->m_sounds.erase(pos);	
		HandleSoundModified(true);
	}	
}
