#pragma once

#pragma once

#include <list>
#include "../GlandaCommandRes/Resource.h"

using namespace std;

class TCommands;
class CScene;

class TCommand
{
protected:
	bool			_M_done;
	unsigned int	_M_Desc_ID;
	//CScene			*_M_pScene;

public:
	TCommand();
	TCommand(unsigned int nDescID);

	virtual ~TCommand();

	virtual bool Execute();
	virtual bool Unexecute();
	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);

	bool Done() const;
	unsigned int GetDescID() const;

	friend class TCommands;
	friend class TCommandGroup;
};

class TCommandGroup : public TCommand
{
public:
	TCommandGroup();
	TCommandGroup(unsigned int nDescID);

	virtual ~TCommandGroup();

	virtual bool Execute();
	virtual bool Unexecute();
	
	void SetDescID(unsigned int id)
	{
		_M_Desc_ID = id;
	}

	unsigned int GetDescID()
	{
		return _M_Desc_ID;
	}

	virtual bool CanMix(TCommand *pOther);
	virtual TCommand *Mix(TCommand *pOther);

	bool Do(TCommand *pCmd);

protected:
	typedef list<TCommand *> CmdContainer;
	typedef list<TCommand *>::iterator CmdIterator;
	typedef list<TCommand *>::reverse_iterator CmdRevIterator;

	list<TCommand *> m_lstCommand;
};

class TCommands
{
public:
	typedef list<TCommand *> CmdContainer;
	typedef list<TCommand *>::iterator CmdIterator;
	typedef list<TCommand *>::reverse_iterator CmdRevIterator;

public:	
	TCommands();
	TCommands(int nMaxSize);
	virtual ~TCommands();

	bool Do(TCommand *command, bool bTemporary = false);
	bool Undo();
	bool Redo();	
	bool CanUndo();
	bool CanRedo();
	int Undo(int nStep);
	int Redo(int nStep);

	void Clear();
	int Size() const;
	void SetMaxSize(int nMaxSize);

	void SetSavePoint();
	unsigned int GetCommandDescID(const CmdIterator &pos);

	CmdIterator Begin();
	CmdIterator End();
	CmdIterator Last();
	CmdIterator GetSavePoint();

protected:
	TCommands(const TCommands &other);
	TCommands &operator=(const TCommands &other);

	bool IsModified();
	CmdIterator NextPosition(const CmdIterator &pos);
	void Expand(int nAdd);	

	bool DoBase(TCommand *command);
	bool UndoBase();
	bool RedoBase();
	int UndoBase(int nStep);
	int RedoBase(int nStep);

	class CGuardSetSETranslator
	{
	public:
		CGuardSetSETranslator(_se_translator_function func)
		{
			m_OldFunc = _set_se_translator(func);
		}

		~CGuardSetSETranslator()
		{
			_set_se_translator(m_OldFunc);
		}

	protected:
		_se_translator_function		m_OldFunc;
	};

	static void OnException(unsigned code, EXCEPTION_POINTERS *ep);
	void HandleError(unsigned int e, TCommand *command, bool bDo);
	static void WriteLine(CFile &file, const CString &str);

protected:
	list<TCommand *>	m_lstCommand;
	CmdIterator			m_iLastCmd;
	CmdIterator			m_iSavePoint;
	bool				m_bSavePointErr;
	int					m_nMaxSize;	
};
