#pragma once

#include "command.h"
#include "CmdUpdateObj.h"
#include "gld_selection.h"
#include "TransAdaptor.h"

using namespace std;

class gldInstance;

class CCmdChangeInstanceMask :
	public TCommand
{
public:
	CCmdChangeInstanceMask(gldInstance *pInstance, BOOL bMask);
	~CCmdChangeInstanceMask(void);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	gldInstance *m_pInstance;
	BOOL m_mask;
};

class CCmdImportVector : public TCommandGroup
{
public:
	CCmdImportVector(gldLibrary *lib, gldMovieClip *mc, const string &name, bool create_inst);
	CCmdImportVector(gldLibrary *lib, gldMovieClip *mc, const string &name, int x, int y);
	virtual ~CCmdImportVector(void);

public:
	virtual bool Execute();
	virtual bool Unexecute();

	static void MergeShape(gldShape *nshape, gldShape *oshape, gldMatrix &mx);
	static gldShape *CreateShapeFromMovie(gldMovieClip *pMovie);

private:
	list<gldObj *>	m_lstObj;
	gldMovieClip	*m_pMC;
	bool			m_bInst;
	string			m_Name;
	int				m_x;
	int				m_y;
	bool			m_bCenter;
};


class CCmdChangeInstanceName : public TCommand
{
public:
	CCmdChangeInstanceName(gldInstance *pInstance, LPCTSTR lpszNewName);
	~CCmdChangeInstanceName();
	virtual bool Execute();
	virtual bool Unexecute();
private:
	gldInstance *m_pInstance;
	std::string m_sNameOld;
	std::string m_sNameNew;
};
