#include "StdAfx.h"
#include "cmdmodifymoviesound.h"
#include "gldMovieSound.h"
#include "gldSound.h"
#include "GlandaDoc.h"

CCmdModifyMovieSound::CCmdModifyMovieSound(gldMovieSound* pMovieSound)
: m_pMovieSound(pMovieSound)
{
	_M_Desc_ID = IDS_CMD_CHANGEMOVIEPROPERTIES;
}

CCmdModifyMovieSound::~CCmdModifyMovieSound(void)
{
	if(m_pMovieSound && _GetMainMovie2()->m_pMovieSound != m_pMovieSound)
	{
		delete m_pMovieSound;
		m_pMovieSound = NULL;
	}
}

bool CCmdModifyMovieSound::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifyMovieSound::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifyMovieSound::ExecSwap()
{
	gldMainMovie2* pMainMovie = _GetMainMovie2();
	std::swap(pMainMovie->m_pMovieSound, m_pMovieSound);
}


CCmdModifyMovieSoundProperties::CCmdModifyMovieSoundProperties(gldMovieSound* pMovieSound, LPCTSTR lpszStartScene, bool bLoopFlag, int nLoopTimes) :
	m_pMovieSound(pMovieSound), 
	m_startScene(lpszStartScene), 
	m_loopTimes(nLoopTimes), 
	m_loopFlag(bLoopFlag)
{
	_M_Desc_ID = IDS_CMD_CHANGEMOVIEPROPERTIES;
}

CCmdModifyMovieSoundProperties::~CCmdModifyMovieSoundProperties(void)
{
}

bool CCmdModifyMovieSoundProperties::Execute()
{
	ExecSwap();
	return TCommand::Execute();
}

bool CCmdModifyMovieSoundProperties::Unexecute()
{
	ExecSwap();
	return TCommand::Unexecute();
}

void CCmdModifyMovieSoundProperties::ExecSwap()
{
	std::swap(m_pMovieSound->m_startScene, m_startScene);
	std::swap(m_pMovieSound->m_sound->m_loopFlag, m_loopFlag);
	std::swap(m_pMovieSound->m_sound->m_loopTimes, m_loopTimes);
}
