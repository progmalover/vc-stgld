#pragma once

#include "gldScene2.h"
#include "gldObj.h"

class gldMovieSound;

class gldMainMovie2
{
public:
	gldMainMovie2(void);
	~gldMainMovie2(void);

public:
	U32 m_width;
	U32 m_height;
	unsigned long m_color;
	float m_frameRate;
	GSCENE2_LIST m_sceneList;

	gldMovieSound*	m_pMovieSound;

private:
	gldScene2 *m_pCurScene;

public:
	int GetEffectCount();
	gldScene2 *GetSceneByFrameNumner(int nFrame);
	int GetSceneStartTime(gldScene2 *pScene);
	gldScene2 *GetCurScene() {return m_pCurScene;}
	gldScene2 *GetScene(int index);
	int GetSceneIndex(gldScene2 *pScene);
	void SetCurScene(gldScene2 *pScene, BOOL bNotify = FALSE);
	BOOL SetCurScene(int index, BOOL bNotify);
	gldScene2 *FindSceneByName(const CString &name);
	int FindSceneIndexByName(const CString &name);
	int GetMaxTime();
	void GetUsedObjs(GOBJECT_LIST &lstObj);

	HRESULT Save(IStorage *pStg, void *pUnk);
	HRESULT Load(IStorage *pStg, void *pUnk);
};

#include "Command.h"
class CCmdChangeMovieProperties : public TCommand
{
public:
	CCmdChangeMovieProperties(float fFrameRate, int nWidth, int nHeight, COLORREF color);	
	virtual ~CCmdChangeMovieProperties();	
	virtual bool Execute();	
	virtual bool Unexecute();	
	//virtual bool CanMix(TCommand *pOther);
	//virtual TCommand *Mix(TCommand *pOther);

private:
	float m_fFrameRate;
	int m_nWidth;
	int m_nHeight;
	COLORREF m_color;

	float m_fFrameRateOld;
	int m_nWidthOld;
	int m_nHeightOld;
	COLORREF m_colorOld;
};
