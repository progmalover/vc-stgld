// EditMCSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "EditMCSheet.h"
#include "gldInstance.h"
#include "gldObj.h"
#include "gldButton.h"
#include "gldMovieClip.h"
#include "gldFrameClip.h"
#include "gldLayer.h"
#include "gldCharacter.h"
#include "gldSound.h"
#include "gldSprite.h"
#include "DrawHelper.h"
#include "my_app.h"
#include "CmdModifyMatrix.h"
#include "CmdModifyCxform.h"


// CEditMCSheet
int _MCData<gldSoundInstance *>::m_nNextUnnamedIndex = 1;

IMPLEMENT_DYNAMIC(CEditMCSheet, CPropertySheet)
CEditMCSheet::CEditMCSheet(gldInstance* pInstance)
:CPropertySheet((pInstance->m_obj->IsGObjInstanceOf(gobjButton) ? IDS_EDIT_BUTTON_SHEET_CAPTION : IDS_EDIT_SPRITE_SHEET_CAPTION), NULL, 0)
	, m_pInstance(pInstance)
	, m_editGeneralPage(pInstance)
	, m_editMatrixPage(pInstance)
	, m_editCxformPage(pInstance)
	//, m_pCmd(NULL)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage(&m_editGeneralPage);

	ReadItemData();
	if(m_texts.size())
	{
		AddPage(&m_editTextPage);
	}
	if(m_shapes.size())
	{
		AddPage(&m_editShapePage);
	}
	if(m_sounds.size())
	{
		AddPage(&m_editSoundPage);
	}
	AddPage(&m_editMatrixPage);
	AddPage(&m_editCxformPage);
}

CEditMCSheet::~CEditMCSheet()
{
	for(std::list<_TextData*>::iterator iText = m_texts.begin(); iText != m_texts.end(); ++iText)
	{
		delete *iText;
	}
	for(std::list<_ShapeData*>::iterator iShape = m_shapes.begin(); iShape != m_shapes.end(); ++iShape)
	{
		delete *iShape;
	}
	for(std::list<_SoundData*>::iterator iSound = m_sounds.begin(); iSound != m_sounds.end(); ++iSound)
	{
		delete *iSound;
	}
}

BEGIN_MESSAGE_MAP(CEditMCSheet, CPropertySheet)
END_MESSAGE_MAP()


// CEditMCSheet message handlers
void CEditMCSheet::ReadItemData()
{
	_ShapeData::ResetUnnamed();
	_TextData::ResetUnnamed();
	_SoundData::ResetUnnamed();

	gldObj* pObj = m_pInstance->m_obj;
	if(pObj->IsGObjInstanceOf(gobjButton))
	{
		gldButton* pButton = (gldButton*)pObj;
		ReadItemData(pButton->m_bmc);
	}
	else if(pObj->IsGObjInstanceOf(gobjSprite))
	{
		gldSprite* pSprite = (gldSprite*)pObj;
		ReadItemData(pSprite->m_mc);
	}
}

void CEditMCSheet::ReadItemData(gldMovieClip* pMovieClip)
{
	GLAYER_LIST& layers = pMovieClip->m_layerList;
	for(GLAYER_LIST::iterator iLayer = layers.begin(); iLayer != layers.end(); ++iLayer)
	{
		gldLayer* pLayer = *iLayer;

		GFRAMECLIP_LIST& frames = pLayer->m_frameClipList;
		int nFrame = 0;
		for(GFRAMECLIP_LIST::iterator iFrame = frames.begin(); iFrame != frames.end(); ++iFrame, ++nFrame)
		{
			gldFrameClip* pFrameClip = *iFrame;

			gldSoundInstance* pSoundInstance = pFrameClip->m_sound;
			if(pSoundInstance != NULL && pSoundInstance->m_soundObj != NULL)
			{
				_SoundData* pSoundData = AfxFindObjectExist(pSoundInstance->m_soundObj, m_sounds);
				if(pSoundData)
				{
					pSoundData->keys.push_back(pSoundInstance);
					pSoundData->frames.insert(_MCFrame(pLayer, nFrame));
				}
				else
				{
					_SoundData* pSoundData = new _SoundData(m_pInstance->m_obj->IsGObjInstanceOf(gobjButton));
					pSoundData->keys.push_back(pSoundInstance);
					pSoundData->frames.insert(_MCFrame(pLayer, nFrame));
					pSoundData->obj = pSoundInstance->m_soundObj;
					m_sounds.push_back(pSoundData);
				}
			}

			GCHARACTERKEY_LIST& keys = pFrameClip->m_characterKeyList;
			for(GCHARACTERKEY_LIST::iterator iKey = keys.begin(); iKey != keys.end(); ++iKey)
			{
				gldCharacterKey* pCharacterKey = *iKey;

				gldObj* pObj = pCharacterKey->GetObj();
				if(pObj->IsGObjInstanceOf(gobjText))
				{
					_TextData* pTextData = AfxFindObjectExist(pObj, m_texts);
					if(pTextData)
					{
						pTextData->keys.push_back(pCharacterKey);
						pTextData->frames.insert(nFrame);
					}
					else
					{
						_TextData* pTextData = new _TextData(m_pInstance->m_obj->IsGObjInstanceOf(gobjButton));
						pTextData->keys.push_back(pCharacterKey);
						pTextData->frames.insert(nFrame);
						pTextData->obj = pObj;
						m_texts.push_back(pTextData);
					}
				}
				else if(pObj->IsGObjInstanceOf(gobjShape))
				{
					_ShapeData* pShapeData = AfxFindObjectExist(pObj, m_shapes);
					if(pShapeData)
					{
						pShapeData->keys.push_back(pCharacterKey);
						pShapeData->frames.insert(nFrame);
					}
					else
					{
						_ShapeData* pShapeData = new _ShapeData(m_pInstance->m_obj->IsGObjInstanceOf(gobjButton));
						pShapeData->keys.push_back(pCharacterKey);
						pShapeData->frames.insert(nFrame);
						pShapeData->obj = pObj;
						m_shapes.push_back(pShapeData);
					}
				}
				else if(pObj->IsGObjInstanceOf(gobjSprite))
				{
					gldSprite* pSprite = (gldSprite*)pObj;
					ReadItemData(pSprite->m_mc);
				}
				else if(pObj->IsGObjInstanceOf(gobjButton))
				{
					gldButton* pButton = (gldButton*)pObj;
					ReadItemData(pButton->m_bmc);
				}
			}
		}
	}
}

BOOL CEditMCSheet::CanEdit()
{
	return m_texts.size() || m_shapes.size() || m_sounds.size();
}

INT_PTR CEditMCSheet::DoModal()
{
	INT_PTR ret = CPropertySheet::DoModal();

	int nCmdCount = 0;
	CGuardDrawOnce xDraw;
	CGuardSelKeeper xSelKeeper;

	if(m_editGeneralPage.m_pCmd)
	{
		++nCmdCount;
		m_editGeneralPage.m_pCmd->Unexecute();
	}
	if(m_editTextPage.m_pCmd)
	{
		++nCmdCount;
		m_editTextPage.m_pCmd->Unexecute();
	}
	if(m_editShapePage.m_pCmd)
	{
		++nCmdCount;
		m_editShapePage.m_pCmd->Unexecute();
	}
	if(m_editSoundPage.m_pCmd)
	{
		++nCmdCount;
		m_editSoundPage.m_pCmd->Unexecute();
	}
	if(m_editMatrixPage.m_pCmd)
	{
		++nCmdCount;
		m_editMatrixPage.m_pCmd->Unexecute();
	}
	if(m_editCxformPage.m_pCmd)
	{
		++nCmdCount;
		m_editCxformPage.m_pCmd->Unexecute();
	}

	if(ret==IDOK)
	{
		if(nCmdCount)
		{
			TCommandGroup* pCmd = new TCommandGroup(IDS_CMD_EDIT_BUTTON_MOVIECLIP);
			if(m_editGeneralPage.m_pCmd)
				pCmd->Do(m_editGeneralPage.m_pCmd);
			if(m_editTextPage.m_pCmd)
				pCmd->Do(m_editTextPage.m_pCmd);
			if(m_editShapePage.m_pCmd)
				pCmd->Do(m_editShapePage.m_pCmd);
			if(m_editSoundPage.m_pCmd)
				pCmd->Do(m_editSoundPage.m_pCmd);
			if(m_editMatrixPage.m_pCmd)
				pCmd->Do(m_editMatrixPage.m_pCmd);
			if(m_editCxformPage.m_pCmd)
				pCmd->Do(m_editCxformPage.m_pCmd);

			//if(m_pCmd == NULL)
			//{
			//	m_pCmd = new TCommandGroup();
			//}
			//m_pCmd->Do(pCmd);
			my_app.Commands().Do(pCmd);
		}
	}
	else
	{
		if(m_editGeneralPage.m_pCmd)
			delete m_editGeneralPage.m_pCmd;
		if(m_editTextPage.m_pCmd)
			delete m_editTextPage.m_pCmd;
		if(m_editShapePage.m_pCmd)
			delete m_editShapePage.m_pCmd;
		if(m_editSoundPage.m_pCmd)
			delete m_editSoundPage.m_pCmd;
		if(m_editMatrixPage)
			delete m_editMatrixPage.m_pCmd;
		if(m_editCxformPage.m_pCmd)
			delete m_editCxformPage;
	}

	return ret;
}
