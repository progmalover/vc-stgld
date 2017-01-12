#pragma once

#include "utils.h"
#include "gldObj.h"
#include "command.h"

class gldScene2;
class gldLibrary;
class gld_shape_sel;

template <class XDraw, class XSel>
class CCmdGroupX : public TCommandGroup
{
public:
	CCmdGroupX(unsigned int nDesc) : TCommandGroup(nDesc) {}

	virtual bool Execute()
	{
		XDraw xDraw;
		XSel  xSel;

		xDraw;
		xSel;

		return TCommandGroup::Execute();
	}

	virtual bool Unexecute()
	{
		XDraw xDraw;
		XSel  xSel;

		xDraw;
		xSel;

		return TCommandGroup::Unexecute();
	}
};

struct GObjPlace
{
	enum
	{
		IN_LIBRARY,
		IS_NEW,
	};
	gldObj *pObj;
	int place;

	GObjPlace(gldObj *paObj = NULL, int aPlace = IN_LIBRARY) : pObj(paObj), place(aPlace) {}
};

struct GObjPlaceList : public list<GObjPlace>
{
	void FreeSymbols();
	void CopyTo(gldLibrary *pLib);
};


gldObj *CreateObject(GObjType type);
BOOL IsReusable(GObjType type);
void DeepSearchRefs(GOBJECT_LIST &lstObj, GOBJECT_LIST::iterator iStart);
void AddObjAndRefs(gldObj *pObj, GOBJECT_LIST &lstObj);
void SortObjsByRef(GOBJECT_LIST &lstObj);
BOOL WriteObjListToBinStream(GOBJECT_LIST &lstObj, oBinStream &os);
BOOL ReadObjListFromBinStream(iBinStream &is, GObjPlaceList &lstObj);
HRESULT WriteSceneToGlobal(gldScene2 *pScene, HGLOBAL *phMem);
HRESULT AddSceneFromGlobal(HGLOBAL hMem, int nIndex);
HRESULT WriteInstancesToGlobal(gld_shape_sel &sel, HGLOBAL *phMem);
HRESULT AddInstancesFromGlobal(HGLOBAL hMem, int dx = 0, int dy = 0);

class CReplaceLibrary
{
public:
	CReplaceLibrary(gldLibrary *pLib);
	~CReplaceLibrary();

protected:
	gldLibrary *m_pOldLib;
};

class CGlandaClipboard
{
public:
	CGlandaClipboard(void);
	virtual ~CGlandaClipboard(void);	

	BOOL Initialize();
	
	void Copy();
	void Paste();
	void Cut();
	void Delete();

	BOOL CopyEffect();
	BOOL CopySound();
	BOOL CopyInstance();
	BOOL CopyScene();
	
	void PasteEffect();
	void PasteSound();
	void PasteInstance();
	void PasteScene();
	
	void CutEffect();
	void CutSound();
	void CutInstance();
	void CutScene();
	
	void DeleteEffect();
	void DeleteSound();
	void DeleteInstance();
	void DeleteScene();

	void PasteDIB();	

	enum
	{
		ForCopy,
		ForCut,
		ForPaste,
		ForDelete
	};

	BOOL IsDataAvailable(int opt = ForCopy);	

	enum
	{
		CbFmtInstance = 0,
		CbFmtScene,
		CbFmtEffect,
		CbFmtSound,
	};

	struct CbFmtEntry
	{
		LPCTSTR	name;
		UINT	id;
	};

	static CbFmtEntry s_FmtMap[];

#ifdef _DEBUG
	static int s_Count;
#endif
};

extern CGlandaClipboard Clipboard;