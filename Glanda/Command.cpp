#include "StdAfx.h"
#include "command.h"
#include "gld_alloc.h"
#include "Observer.h"
#include "GlandaDoc.h"
#include "Global.h"
#include "filepath.h"
#include "DrawHelper.h"
#include "resource.h"

TCommand::TCommand()
: _M_done(false)
, _M_Desc_ID(0)
{
}

TCommand::TCommand(unsigned int nDescID)
: _M_done(false)
, _M_Desc_ID(nDescID)
{
}

TCommand::~TCommand()
{	
}

bool TCommand::Execute()
{
	_M_done = true;
	return true;
}

bool TCommand::Unexecute()
{		
	_M_done = false;
	return true;
}

bool TCommand::Done() const
{
	return _M_done;
}

bool TCommand::CanMix(TCommand *pOther)
{
	return false;
}

TCommand *TCommand::Mix(TCommand *pOther)
{
	return NULL;
}

unsigned int TCommand::GetDescID() const
{
	return _M_Desc_ID;
}
/*******************************************************************
	Class TCommandGroup

\******************************************************************/
TCommandGroup::TCommandGroup()
{
}

TCommandGroup::TCommandGroup(unsigned int nDescID)
: TCommand(nDescID)
{
}

TCommandGroup::~TCommandGroup()
{
	for (CmdIterator i = m_lstCommand.begin(); i != m_lstCommand.end(); ++i)
	{
		delete (*i);
	}
}

bool TCommandGroup::Execute()
{
	if (m_lstCommand.size() > 0)
	{
		if (!_M_done)
		{
			for (CmdIterator i = m_lstCommand.begin(); i != m_lstCommand.end(); ++i)
			{
				(*i)->Execute();
			}

			return TCommand::Execute();
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool TCommandGroup::Unexecute()
{
	ASSERT(m_lstCommand.size() > 0 && _M_done);

	for (CmdRevIterator i = m_lstCommand.rbegin(); i != m_lstCommand.rend(); ++i)
	{
		(*i)->Unexecute();
	}

	return TCommand::Unexecute();
}

bool TCommandGroup::CanMix(TCommand *pOther)
{
	if (m_lstCommand.empty())
	{
		return false;
	}
	else
	{
		ASSERT(pOther->_M_done && _M_done);

		return (*(--m_lstCommand.end()))->CanMix(pOther);
	}
}

TCommand *TCommandGroup::Mix(TCommand *pOther)
{
	if (m_lstCommand.empty())
	{
		return NULL;
	}
	else
	{
		ASSERT(pOther->_M_done && _M_done);

		TCommand *pCmd = (*(--m_lstCommand.end()))->Mix(pOther);

		if (pCmd)
		{
			m_lstCommand.erase(--m_lstCommand.end());
			m_lstCommand.push_back(pCmd);

			return this;
		}
		else
		{
			return NULL;
		}		
	}
}

bool TCommandGroup::Do(TCommand *pCmd)
{
	if (pCmd->Execute())
	{
		// mix command
		CmdIterator iLast = --m_lstCommand.end();
		if (iLast != m_lstCommand.end())
		{
			TCommand *pLast = *iLast;
			TCommand *pComp = pLast->Mix(pCmd);
			if (pComp)
			{
				m_lstCommand.erase(iLast);
				pCmd = pComp;
			}
		}

		// push command to container
		m_lstCommand.push_back(pCmd);
		_M_done = true;

		return true;
	}

	delete pCmd;

	return false;
}

/*******************************************************************
	Class TCommands

\******************************************************************/
TCommands::TCommands()
{
	m_nMaxSize = 32767;
	m_iLastCmd = m_lstCommand.end();
	m_iSavePoint = m_lstCommand.end();	
	m_bSavePointErr = false;
}

TCommands::TCommands(int nMaxSize)
{
	ASSERT(nMaxSize > 0);

	m_nMaxSize = (nMaxSize < 1 ? 32767 : nMaxSize);
	m_iLastCmd = m_lstCommand.end();
	m_iSavePoint = m_lstCommand.end();	
	m_bSavePointErr = false;
}

TCommands::TCommands(const TCommands &other)
{
	ASSERT(false);
}

TCommands &TCommands::operator=(const TCommands &other)
{
	ASSERT(false);

	return *this;
}

TCommands::~TCommands()
{
	for (CmdIterator i = m_lstCommand.begin(); i != m_lstCommand.end(); ++i)
	{
		//if ((*i)->_M_pScene)
		//{
		//	delete (*i)->_M_pScene;
		//}

		delete (*i);
	}
}

bool TCommands::Do(TCommand *command, bool bTemporary)
{
	CGuardSetSETranslator xe(OnException);

	try
	{
		return DoBase(command);
	}
	catch(unsigned int e)
	{
		HandleError(e, command, true);
	}

	return false;
}

bool TCommands::Undo()
{
	CGuardSetSETranslator xe(OnException);

	try
	{
		return UndoBase();
	}
	catch(unsigned int e)
	{
		HandleError(e, NULL, false);
	}

	return false;
}

bool TCommands::Redo()
{
	CGuardSetSETranslator xe(OnException);

	try
	{
		return RedoBase();
	}
	catch(unsigned int e)
	{
		HandleError(e, NULL, true);
	}

	return false;
}

int TCommands::Undo(int nStep)
{
	CGuardSetSETranslator xe(OnException);

	try
	{
		return UndoBase(nStep);
	}
	catch(unsigned int e)
	{
		HandleError(e, NULL, false);
	}

	return false;
}

int TCommands::Redo(int nStep)
{
	CGuardSetSETranslator xe(OnException);

	try
	{
		return RedoBase(nStep);
	}
	catch(unsigned int e)
	{
		HandleError(e, NULL, true);
	}

	return false;
}

bool TCommands::UndoBase()
{	
	ASSERT(m_iLastCmd != m_lstCommand.end());	

	if (m_iLastCmd != m_lstCommand.end())
	{
		CGuardDrawOnce	xDraw;

		// Restore current scene
		//CScene *pCurScene = (*m_iLastCmd)->_M_pScene;
		//ASSERT(pCurScene != NULL);
		//if (!pCurScene->IsCurrent())
		//{
		//	CStaticNav::Instance()->Set(pCurScene);
		//}

		// If previous command's execute scene is null put current scene to it
		CmdIterator	iPrev = m_iLastCmd;
		iPrev--;
		//if (iPrev != m_lstCommand.end() && (*iPrev)->_M_pScene == NULL)
		//{
		//	(*iPrev)->_M_pScene = pCurScene;
		//	(*m_iLastCmd)->_M_pScene = NULL;
		//}		

		// Unexecute last command
		bool bRet = (*m_iLastCmd)->Unexecute();

		// Update data
		m_iLastCmd = iPrev;

		CSubjectManager::Instance()->GetSubject("ModifyDocument")->Notify((void *)IsModified());

		return bRet;
	}	

	return false;
}

int TCommands::UndoBase(int nStep)
{
	int	nRet = 0;

	//_GetCurDocument()->SetUpdate(false);

	while ((m_iLastCmd != m_lstCommand.end()) && (nStep > nRet))
	{
		// Restore current scene
		//CScene *pCurScene = (*m_iLastCmd)->_M_pScene;
		//ASSERT(pCurScene != NULL);
		//if (!pCurScene->IsCurrent())
		//{
		//	CStaticNav::Instance()->Set(pCurScene);
		//}

		// If previous command's execute scene is null put current scene to it
		CmdIterator	iPrev = m_iLastCmd;
		iPrev--;
		//if (iPrev != m_lstCommand.end() && (*iPrev)->_M_pScene == NULL)
		//{
		//	(*iPrev)->_M_pScene = pCurScene;
		//	(*m_iLastCmd)->_M_pScene = NULL;
		//}

		(*(m_iLastCmd--))->Unexecute();

		nRet++;		
	}

	//_GetCurDocument()->SetUpdate(true);

	CSubjectManager::Instance()->GetSubject("ModifyDocument")->Notify((void *)IsModified());

	return nRet;
}

bool TCommands::RedoBase()
{
	CmdIterator	iNextCmd = NextPosition(m_iLastCmd);

	ASSERT(iNextCmd != m_lstCommand.end());

	if (iNextCmd != m_lstCommand.end())
	{
		// Restore current scene		
		//CScene *pCurScene = (*iNextCmd)->_M_pScene;
		//if (pCurScene == NULL)
		//{
		//	ASSERT(m_iLastCmd != m_lstCommand.end());
		//	ASSERT((*m_iLastCmd)->_M_pScene != NULL);

		//	pCurScene = (*m_iLastCmd)->_M_pScene;
		//	(*m_iLastCmd)->_M_pScene = NULL;
		//	(*iNextCmd)->_M_pScene = pCurScene;
		//}

		CGuardDrawOnce	xDraw;

		//if (!pCurScene->IsCurrent())
		//{
		//	CStaticNav::Instance()->Set(pCurScene);
		//}				

		// Execute next command
		bool bRet = (*iNextCmd)->Execute();

		m_iLastCmd = iNextCmd;

		CSubjectManager::Instance()->GetSubject("ModifyDocument")->Notify((void *)IsModified());		

		return bRet;
	}	

	return false;
}

int TCommands::RedoBase(int nStep)
{
	CmdIterator	iNextCmd = NextPosition(m_iLastCmd);
	int			nRet = 0;
	
	//_GetCurDocument()->SetUpdate(false);

	while ((iNextCmd != m_lstCommand.end()) && (nStep > nRet))
	{
		// Restore current scene		
		//CScene *pCurScene = (*iNextCmd)->_M_pScene;
		//if (pCurScene == NULL)
		//{
		//	ASSERT(m_iLastCmd != m_lstCommand.end());
		//	ASSERT((*m_iLastCmd)->_M_pScene != NULL);
		//	pCurScene = (*m_iLastCmd)->_M_pScene;
		//	(*m_iLastCmd)->_M_pScene = NULL;
		//	(*iNextCmd)->_M_pScene = pCurScene;
		//}
		//if (!pCurScene->IsCurrent())
		//{
		//	CStaticNav::Instance()->Set(pCurScene);
		//}

		// Execute next command
		(*(iNextCmd++))->Execute();

		m_iLastCmd++;

		++nRet;
	}	

	//_GetCurDocument()->SetUpdate(true);

	CSubjectManager::Instance()->GetSubject("ModifyDocument")->Notify((void *)IsModified());
	
	return nRet;
}

bool TCommands::CanUndo()
{
	return m_iLastCmd != m_lstCommand.end();
}

bool TCommands::CanRedo()
{
	return NextPosition(m_iLastCmd) != m_lstCommand.end();
}

void TCommands::Clear()
{
	m_bSavePointErr = IsModified();

	for (CmdIterator i = m_lstCommand.begin(); i != m_lstCommand.end(); ++i)
	{
		//if ((*i)->_M_pScene)
		//{
		//	delete (*i)->_M_pScene;
		//}

		delete (*i);
	}

	m_lstCommand.clear();

	m_iLastCmd = m_lstCommand.end();
	m_iSavePoint = m_lstCommand.end();	
}

bool TCommands::DoBase(TCommand *command)
{
	ASSERT(command != NULL);

	// Save current scene to command
	//CScene *pObjScene = new CScene;
	//pObjScene->GetCurrent();	

	if (command->Execute())	// because command execute may be change scene so save it first
	{
		//CScene *pLastScene = NULL;

		//if (m_iLastCmd != m_lstCommand.end())
		//{
		//	pLastScene = (*m_iLastCmd)->_M_pScene;
		//	ASSERT(pLastScene != NULL);
		//}

		//if (pLastScene == NULL)
		//{
		//	pLastScene = pObjScene;
		//}
		//else
		//{
		//	if (*pLastScene == *pObjScene)
		//	{				
		//		(*m_iLastCmd)->_M_pScene = NULL;
		//		delete pObjScene;
		//	}
		//	else
		//	{
		//		pLastScene = pObjScene;
		//	}
		//}
		//command->_M_pScene = pLastScene;

		// Clear redo record
		CmdIterator i = NextPosition(m_iLastCmd);

		while (i != m_lstCommand.end())
		{
			if ((m_bSavePointErr == false) && (i == m_iSavePoint))
			{
				m_bSavePointErr = true;
			}
			//if ((*i)->_M_pScene != NULL)
			//{
			//	delete (*i)->_M_pScene;
			//}
			delete (*i);
			m_lstCommand.erase(i++);
		}

		// Mix command
		if (m_iLastCmd != m_lstCommand.end() /*&& (*m_iLastCmd)->_M_pScene == NULL*/) // Can mixed command must be on same scene
		{
			TCommand *pNew = (*m_iLastCmd)->Mix(command);

			if (pNew)
			{
				if ((m_bSavePointErr == false) && (m_iLastCmd == m_iSavePoint))
				{
					m_bSavePointErr = true;
				}

				//pNew->_M_pScene = pLastScene;
				pNew->_M_done = true;				

				m_lstCommand.erase(m_iLastCmd);
				m_lstCommand.push_back(pNew);

				m_iLastCmd = --m_lstCommand.end();				
			}
			else
			{
				Expand(1);

				m_lstCommand.push_back(command);
				m_iLastCmd = --m_lstCommand.end();
			}
		}
		else
		{	// Command list is empty
			Expand(1);

			m_lstCommand.push_back(command);
			m_iLastCmd = --m_lstCommand.end();
		}

		CSubjectManager::Instance()->GetSubject("ModifyDocument")->Notify((void *)IsModified());

		return true;
	}			

	//delete pObjScene;
	delete command;

	return false;
}

void TCommands::Expand(int nAdd)
{
	int nRemove = nAdd + (int)m_lstCommand.size() - m_nMaxSize;

	while (nRemove > 0)
	{
		CmdIterator i = m_lstCommand.begin();
		if (i == m_iSavePoint)
		{
			m_bSavePointErr = true;
		}
		delete (*i);
		m_lstCommand.erase(i);
		--nRemove;
	}
}

TCommands::CmdIterator TCommands::NextPosition(const CmdIterator &pos)
{
	CmdIterator next = pos;

	return ++next;
}

bool TCommands::IsModified()
{
	return m_bSavePointErr || (m_iLastCmd != m_iSavePoint);
}

void TCommands::SetSavePoint()
{
	m_iSavePoint = m_iLastCmd;
	m_bSavePointErr = false;
}

TCommands::CmdIterator TCommands::GetSavePoint()
{
	if (m_bSavePointErr)
	{
		return m_lstCommand.end();
	}
	else
	{
		return m_iSavePoint;
	}
}

unsigned int TCommands::GetCommandDescID(const TCommands::CmdIterator &pos)
{
	assert(pos != m_lstCommand.end());
	return (*pos)->_M_Desc_ID;
}

TCommands::CmdIterator TCommands::Begin()
{
	return m_lstCommand.begin();
}

TCommands::CmdIterator TCommands::Last()
{
	return m_iLastCmd;
}

TCommands::CmdIterator TCommands::End()
{
	return m_lstCommand.end();
}

void TCommands::SetMaxSize(int nMaxSize)
{
	ASSERT(nMaxSize > 0);

	if (nMaxSize != m_nMaxSize)
	{
		m_nMaxSize = nMaxSize;
		CmdIterator	pos = m_lstCommand.begin();
		while ((int)m_lstCommand.size() > m_nMaxSize && pos != m_iLastCmd)
		{
			if ((m_bSavePointErr == false) && (pos == m_iSavePoint))
			{
				m_bSavePointErr = true;
			}
			delete (*pos);
			m_lstCommand.erase(pos++);
		}
		pos = --m_lstCommand.end();
		while ((int)m_lstCommand.size() > m_nMaxSize && pos != m_iLastCmd)
		{
			if ((m_bSavePointErr == false) && (pos == m_iSavePoint))
			{
				m_bSavePointErr = true;
			}
			delete (*pos);
			m_lstCommand.erase(pos--);
		}
	}
}

void TCommands::OnException(unsigned code, EXCEPTION_POINTERS *ep)
{
#ifndef _DEBUG
	throw code;
#endif
}

void TCommands::HandleError(unsigned int e, TCommand *command, bool bDo)
{
	if (!command)
	{
		command = (m_iLastCmd == m_lstCommand.end()) ? NULL : (*m_iLastCmd);	
	}

	CString	sTips;
	CString strCommand;
	CString strExecute;
	if (command)
	{
		::GetCommandDesc(command->GetDescID(), strCommand);
		strExecute.Format(bDo ? "execute command '%s' (0x%08x)" : "unexecute command '%s' (0x%08x)", (LPCTSTR)strCommand, e);
	}
	else
	{
		strExecute.Format("execute a null command (0x%08x)", e);
	}

	// Write error log
	CString strFile = ::GetModuleFilePath();
	strFile += "\\error.log";

	sTips.Format(IDS_PROMPT_COMMAND_ERROR2, 
		(LPCTSTR)strExecute, (LPCTSTR)strFile);

	CStdioFile	file(strFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);	
	WriteLine(file, "Exception caught while " + strExecute);
	
	CString strBegin = "[BEGIN COMMAND LIST]";
	WriteLine(file, strBegin);
	for (CmdIterator i = m_lstCommand.begin(); i != m_lstCommand.end(); ++i)
	{
		if (*i)
		{
			::GetCommandDesc((*i)->GetDescID(), strCommand);
		}
		else
		{
			strCommand = "Null command";
		}

		if (i == m_iLastCmd)
		{
			strCommand = strCommand + "			<Current>";
		}

		WriteLine(file, strCommand);
	}
	CString strEnd = "[END COMMAND LIST]";
	WriteLine(file, strEnd);

	file.Flush();

	AfxMessageBox(sTips, MB_OK | MB_ICONERROR);
}

void TCommands::WriteLine(CFile &file, const CString &str)
{
	const TCHAR crlf[] = "\r\n";

	file.Write(str, str.GetLength());
	file.Write(crlf, sizeof(crlf) - sizeof(TCHAR));
}
