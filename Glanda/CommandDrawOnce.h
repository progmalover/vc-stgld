#pragma once
#include "command.h"
#include "DrawHelper.h"

class CCommandDrawOnce : public TCommandGroup
{
public:
	CCommandDrawOnce(void);
	CCommandDrawOnce(unsigned int nDescID);
	virtual ~CCommandDrawOnce(void);

	virtual bool Execute()
	{
		CGuardDrawOnce xDrawOnce;
		return TCommandGroup::Execute();
	}
	virtual bool Unexecute()
	{
		CGuardDrawOnce xDrawOnce;
		return TCommandGroup::Unexecute();
	}
};

//////////////////////////////////////////////////////////////////////////
// Do��������RebuildCurrentScene,����ʹ�ô����Do֮��Ҫ�Լ�����EndDo
//////////////////////////////////////////////////////////////////////////

template <class T>
class CCommandLastRebuild : public T
{
public:
	CCommandLastRebuild<T>()
	{
	}
	CCommandLastRebuild<T>(unsigned int nDescID)
	: T(nDescID)
	{
	}
	virtual ~CCommandLastRebuild<T>()
	{
	}

	virtual bool Execute()
	{
		bool bRet = T::Execute();
		EndDo();
		return bRet;
	}
	virtual bool Unexecute()
	{
		bool bRet = T::Unexecute();
		EndDo();
		return bRet;
	}
	void EndDo()
	{
		CTransAdaptor::RebuildCurrentScene();
	}
};

typedef CCommandLastRebuild<CCommandDrawOnce> CCommandBuildDrawOnce;