#include "StdAfx.h"
#include "cmdreplacesound.h"
#include "gldSound.h"
#include "TransAdaptor.h"

CCmdReplaceSound::CCmdReplaceSound(gldSound* pSoundObj, gldSoundInstance* pSoundInstance)
: m_pSoundObj(pSoundObj)
, m_pSoundInstance(pSoundInstance)
{
	_M_Desc_ID = IDS_CMD_REPLACESOUND;
}

CCmdReplaceSound::~CCmdReplaceSound(void)
{
}

bool CCmdReplaceSound::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdReplaceSound::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdReplaceSound::ExecSwap()
{
	std::swap(m_pSoundInstance->m_soundObj, m_pSoundObj);
}