#pragma once

#include "Command.h"
#include "gld_graph.h"
#include "gldMatrix.h"
#include "gldCxform.h"
#include "gld_selection.h"
#include "Matrix.h"
#include "gldInstance.h"

#include <list>

class gldScene2;
class gldObj;

class CCmdAddObj : public TCommand
{
public:
	CCmdAddObj(gldObj *pObj);
	~CCmdAddObj(void);
	virtual bool Execute();
	virtual bool Unexecute();
private:
	gldObj *m_obj;
	ULONG m_nRef;
};

class CCmdRemoveObj : public TCommand
{
public:
	CCmdRemoveObj(gldObj* obj);
	~CCmdRemoveObj();

public:
	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldObj * m_obj;

	ULONG m_nRef;
};

class CCmdAddInstance : public TCommand
{
public:
	CCmdAddInstance(gldScene2 *pScene, gldObj *pObj, const gldMatrix &matrix, const gldCxform &cxform);
	CCmdAddInstance(gldScene2 *pScene, gldInstance *pInst, bool bAutoDelete = true);
	~CCmdAddInstance(void);
	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldScene2 *m_scene;
	gldInstance *m_instance;
	bool m_bAutoDelete;
};

class CCmdTransformInstance : public TCommand
{
public:
	CCmdTransformInstance(const gld_shape_sel &sel, const gld_list<TMatrix> &matrixList);
	~CCmdTransformInstance(void);
	virtual bool Execute();
	virtual bool Unexecute();
	void Do();
	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);

private:
	GINSTANCE_LIST m_instanceList;
	gld_list<TMatrix> m_matrixList;
};

class CCmdMoveInstance : public TCommand
{
public:
	CCmdMoveInstance(const gld_shape_sel &sel, int dx, int dy);
	~CCmdMoveInstance(void);
	virtual bool Execute();
	virtual bool Unexecute();
	void Do(int dx, int dy);
	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);

private:
	GINSTANCE_LIST m_instanceList;
	int m_dx;
	int m_dy;
};

class CCmdAddShape : public TCommandGroup
{
public:
	CCmdAddShape(gld_shape &shape);
	virtual ~CCmdAddShape(void);

	virtual bool Execute();
	virtual bool Unexecute();
};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class gldText2;
class CCmdAddText : public TCommandGroup
{
public:
	CCmdAddText(gldText2 *pText);
	virtual ~CCmdAddText(void);

	virtual bool Execute();
	virtual bool Unexecute();
};
