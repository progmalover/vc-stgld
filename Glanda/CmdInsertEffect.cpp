#include "StdAfx.h"
#include "TextToolEx.h"
#include "cmdinserteffect.h"
#include "gldInstance.h"
#include "gldEffect.h"
#include "EffectWnd.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldCharacter.h"
#include "SWFProxy.h"
#include "gldSprite.h"
#include "my_app.h"
#include "EffectCommonParameters.h"
#include "CmdAddShape.h"
#include "GlandaDoc.h"
#include "MoviePreview.h"


CCmdInsertEffect::CCmdInsertEffect(gldInstance* pInstance, gldEffect* pEffect, gldEffect* pEffectInsertBefore)
: m_pInstance(pInstance)
, m_pEffect(pEffect)
, m_pEffectInsertBefore(pEffectInsertBefore)
{
	_M_Desc_ID = IDS_CMD_ADDEFFECT;
}

CCmdInsertEffect::~CCmdInsertEffect(void)
{
	if(!Done())
	{
		delete m_pEffect;
		m_pEffect = NULL;
	}
}

bool CCmdInsertEffect::Execute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	effectList.insert(std::find(effectList.begin(), effectList.end(), m_pEffectInsertBefore), m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	CEffectWnd::Instance()->SetEffectSelection(m_pInstance, m_pEffect, TRUE);

	return TCommand::Execute();
}

bool CCmdInsertEffect::Unexecute()
{
	GEFFECT_LIST& effectList = m_pInstance->m_effectList;
	if(effectList.size() > 1)
	{
		GEFFECT_LIST::iterator iter = std::find(effectList.begin(), effectList.end(), m_pEffect);
		if(++iter == effectList.end())
		{
			--iter;
			--iter;
		}
		CEffectWnd::Instance()->SetEffectSelection(m_pInstance, *iter, TRUE);
	}
	else
	{
		CEffectWnd::Instance()->SetInstSelection(m_pInstance, TRUE);
	}

	m_pInstance->m_effectList.remove(m_pEffect);
	CSubjectManager::Instance()->GetSubject("ChangeEffect")->Notify(0);

	return TCommand::Unexecute();
}

// class CCmdInsertTextEffect
CCmdInsertTextEffect::CCmdInsertTextEffect(const CString &text, CTextFormat &txtFmt
										   , gldEffect *pEffect, IGLD_Parameters *options
										   , const CString &strURL /*= ""*/, const CString &strTarget /*= ""*/)
: TCommandGroup(IDS_CMD_INSERTTEXTEFFECT)
{
	gldText2 *pText = CreateSingleLineText2(text, txtFmt);
	m_lstCommand.push_back(new CCmdAddObj(pText));
	gldScene2 *pScene = _GetCurScene2();
	gldMatrix matrix;
	matrix.m_x = ((int)_GetMainMovie2()->m_width * 20 - pText->m_bounds.Width()) / 2;
	matrix.m_y = ((int)_GetMainMovie2()->m_height * 20 - pText->m_bounds.Height()) / 2;
	gldInstance *pInst = new gldInstance(pText, matrix, gldCxform());
	CopyParameters(options, PT_COMMON, pInst->m_pIParas);
	m_lstCommand.push_back(new CCmdAddInstance(pScene, pInst));
	if (pEffect != NULL)
	{
		CopyParameters(options, PT_CUSTOM, pEffect->m_pIParas);
		pEffect->CalcIdealLength(pInst->GetEffectKeyCount());
		m_lstCommand.push_back(new CCmdInsertEffect(pInst, pEffect, NULL));
	}
	if (strURL.GetLength() > 0)
	{
		CString strAction;
		strAction.Format("//{{GLANDA_EVENT\r\n"
			"on(release)\r\n"
			"{\r\n"
			"_root.getURL(\"%s\", \"%s\");\r\n"
			"}\r\n"
			"//}}GLANDA_EVENT\r\n", (LPCTSTR)strURL, (LPCTSTR)strTarget);
		m_lstCommand.push_back(new CCmdSetAction(pInst, (LPCTSTR)strAction));
	}
}

CCmdInsertTextEffect::~CCmdInsertTextEffect()
{
}

bool CCmdInsertTextEffect::Execute()
{
	return TCommandGroup::Execute();
}

bool CCmdInsertTextEffect::Unexecute()
{
	return TCommandGroup::Unexecute();
}