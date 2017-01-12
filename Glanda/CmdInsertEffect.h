#pragma once
#include "command.h"

class gldInstance;
class gldEffect;
interface IGLD_Parameters;
class gldText2;
class CTextFormat;

class CCmdInsertEffect : public TCommand
{
protected:
	gldEffect* m_pEffect;

	gldInstance* m_pInstance;
	// 当前插入点的特效,若插入到最后,则应为NULL
	gldEffect* m_pEffectInsertBefore;
public:
	CCmdInsertEffect(gldInstance* pInstance, gldEffect* pEffect, gldEffect* pEffectInsertBefore);
	virtual ~CCmdInsertEffect(void);

	virtual bool Execute();
	virtual bool Unexecute();
};

// class CCmdInsertTextEffect for insert text width effect
class CCmdInsertTextEffect : public TCommandGroup
{
public:
	CCmdInsertTextEffect(const CString &text, CTextFormat &txtFmt
		, gldEffect *pEffect, IGLD_Parameters *options
		, const CString &strURL = "", const CString &strTarget = "");
	virtual ~CCmdInsertTextEffect();

	virtual bool Execute();
	virtual bool Unexecute();
};