#pragma once

#include "gld_graph.h"
#include "Command.h"
#include "gldMatrix.h"
#include "gldCxform.h"
#include "gldInstance.h"

class gldObj;
class gldScene2;
class CBackground
{
public:
	enum 
	{
		POS_CENTER = 0,
		POS_STRETCH,		
		POS_TOPLEFT,
		POS_TOPRIGHT,
		POS_BOTTOMLEFT,
		POS_BOTTOMRIGHT,
		POS_CUSTOM,
	};

public:	
	CBackground(gldObj *pObj, int pos, const gldMatrix &mx = gldMatrix(), const gldCxform &cx = gldCxform());
	CBackground(gldInstance *pInst, int pos);
	virtual ~CBackground(void);

	inline gldObj *GetObj() const { return m_pInst->m_obj; }
	inline int GetPosition() const { return m_Pos; }
	void SetPosition(int pos) { m_Pos = pos; }
	gld_shape GetEditShape(bool update = true);
	void Update();
	inline gldMovieClip *GetMovieClip() { return m_pInst->m_mc; }
	gldMatrix GetMatrix();
	inline gldCxform GetCxform() { return m_pInst->m_cxform; }
	inline std::string &GetName() { return m_pInst->m_name; }
	inline std::string &GetAction() { return m_pInst->m_action; }	
	BOOL BuildEffects(gldInstance::EnumErrorHandleMethod method = gldInstance::IS_PROMPT);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);
	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	static CBackground *FromStream(IStream *pStm, void *pUnk);
	gldInstance *Detach();
	void Attach(gldInstance *pInst);

private:
	CBackground(void);

	void DestroyShape();

private:
	int m_Pos;
	gld_shape m_Shape;
	gldInstance *m_pInst;	
};

class CCmdChangeSceneBackgrnd : public TCommand
{
public:
	CCmdChangeSceneBackgrnd(gldScene2 *scene, gldObj *obj, int pos
		, const gldMatrix &mx = gldMatrix(), const gldCxform &cx = gldCxform());
	CCmdChangeSceneBackgrnd(gldScene2 *scene, gldInstance *pInst, int pos, bool attached);
	CCmdChangeSceneBackgrnd(gldScene2 *scene);
	virtual ~CCmdChangeSceneBackgrnd();
	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldScene2 *m_pScene;
	CBackground *m_pBackgrnd;
	gldInstance *m_pInst;
	bool m_bAttached;
};

class CCmdChangeBackgrndPos : public TCommand
{
public:
	CCmdChangeBackgrndPos(CBackground *backgrnd, int pos);
	virtual bool Execute();
	virtual bool Unexecute();

private:
	void Do();

	CBackground *m_Backgrnd;
	int m_Pos;	
};