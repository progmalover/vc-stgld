#pragma once

#include "CmdUpdateObj.h"
#include <vector>
#include "gldMatrix.h"
#include <stack>
#include "gldCharacter.h"
#include "gldFrameClip.h"
#include "gldMovieClip.h"

class gldButton;
class gldText2;
class CCmdChangeButtonCap : public CCmdGroupUpdateObjUniId
{
public:
	CCmdChangeButtonCap(gldButton *button, const CString &strOldText, const CString &strNewText, int method);
	virtual ~CCmdChangeButtonCap(void);

	enum
	{
		KEEP_BOUND,
		KEEP_SIZE,
	};

	static CString GetButtonCaption(gldButton *button);
	static CString GetTextString(gldText2 *text);
	static bool PromptChangeComplexText(gldButton *button, const CString &strCap);

	virtual bool Execute();
	virtual bool Unexecute();

private:
	void Assign();

private:
	struct KeyBackup
	{
		gldCharacterKey *key;		
		gldMatrix m;
		gldText2 *text;
	};
	gldButton *m_pButton;
	CString m_strNewText;
	CString m_strOldText;
	int m_Method;
	std::vector<KeyBackup> m_Backup;
};



/* CKeyIterator */

class CKeyIterator
{
public:
	CKeyIterator(gldMovieClip *mc);
	bool Next();
	gldCharacterKey *operator*() { return *ikey; }

private:
	bool Init();

private:
	gldMovieClip *m_mc;
	bool m_bInit;
	GLAYER_LIST_IT ila;
	GFRAMECLIP_LIST_IT ifc;
	GCHARACTERKEY_LIST_IT ikey;
};


/* CTextIterator */

class CTextKeyIterator
{
public:
	CTextKeyIterator(gldMovieClip *mc);
	bool Next();
	gldCharacterKey *operator*() { return m_pKey; }
	gldText2 *GetText() { return (gldText2 *)m_pKey->GetObj(); }
	
private:
	std::stack<CKeyIterator> m_Stack;
	gldCharacterKey *m_pKey;
};