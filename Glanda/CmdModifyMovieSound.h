#pragma once

#include "command.h"
#include <string>

class gldMovieSound;

class CCmdModifyMovieSound : public TCommand
{
	gldMovieSound* m_pMovieSound;
public:
	CCmdModifyMovieSound(gldMovieSound* pMovieSound);
	virtual ~CCmdModifyMovieSound(void);

	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
};

class CCmdModifyMovieSoundProperties : public TCommand
{
	gldMovieSound* m_pMovieSound;
	std::string m_startScene;
	bool m_loopFlag;
	int m_loopTimes;
public:
	CCmdModifyMovieSoundProperties(gldMovieSound* pMovieSound, LPCTSTR lpszStartScene, bool bLoopFlag, int nLoopTimes);
	virtual ~CCmdModifyMovieSoundProperties(void);

	virtual bool Execute();
	virtual bool Unexecute();
	void ExecSwap();
};
