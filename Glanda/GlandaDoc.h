// GlandaDoc.h : interface of the CGlandaDoc class
//
#pragma once
#include <list>
#include "Observer.h"
#include "SWFParse.h"
#include "Global.h"
#include "gldDataKeeper.h"
#include "gldMainMovie2.h"
#include "ObjectMap.h"

class gldObj;
class CSWFParse;
class gldMovieClip;
class CScene;
class gldLayer;
struct TShapeEditorLayout;

class gldScene2;

class CGlandaDoc : public CDocument
{
protected: // create from serialization only
	CGlandaDoc();
	DECLARE_DYNCREATE(CGlandaDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
//	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CGlandaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CString m_strMovieExport;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	virtual void SetModifiedFlag(BOOL bModified);

public:
	afx_msg void OnPublish();
	afx_msg void OnFileExportMovie();
	afx_msg void OnNewWindow();
	afx_msg void OnFilePreview();

public:
	BOOL ImportImage(LPCTSTR lpszFile, CPoint point, BOOL bShowImportDialog = FALSE);
	BOOL ImportGLS(LPCTSTR lpszFile, CPoint point);
	BOOL ImportVectorFormat(LPCTSTR lpszFile, CPoint point);
	BOOL ImportSWF(LPCTSTR lpszFile);
	BOOL ImportGLC(LPCTSTR lpszFile, CPoint point);
	BOOL ImportSound(LPCTSTR lpszFileName);


	CString GetExportMovieName();

protected:
	//DECLARE_GLANDA_OBSERVER(CGlandaDoc, Select);
	DECLARE_GLANDA_OBSERVER(CGlandaDoc, ModifyDocument)

protected:
	void DoFileImport();
	
public:
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);

public:
	afx_msg void OnFormatAlighlefts();
	afx_msg void OnUpdateFormatAlighlefts(CCmdUI *pCmdUI);
	afx_msg void OnFormatAlightops();
	afx_msg void OnUpdateFormatAlightops(CCmdUI *pCmdUI);
	afx_msg void OnFormatAlighrights();
	afx_msg void OnUpdateFormatAlighrights(CCmdUI *pCmdUI);
	afx_msg void OnFormatAlighbottoms();
	afx_msg void OnUpdateFormatAlighbottoms(CCmdUI *pCmdUI);
	afx_msg void OnFormatAlighcenters();
	afx_msg void OnUpdateFormatAlighcenters(CCmdUI *pCmdUI);
	afx_msg void OnFormatAlighmiddles();
	afx_msg void OnUpdateFormatAlighmiddles(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakesamewidth();
	afx_msg void OnUpdateFormatMakesamewidth(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakesameheight();
	afx_msg void OnUpdateFormatMakesameheight(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakesamesize();
	afx_msg void OnUpdateFormatMakesamesize(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakehorizontalspacingequal();
	afx_msg void OnUpdateFormatMakehorizontalspacingequal(CCmdUI *pCmdUI);
	afx_msg void OnFormatMakeverticalspacingequal();
	afx_msg void OnUpdateFormatMakeverticalspacingequal(CCmdUI *pCmdUI);
	afx_msg void OnFormatBringtofront();
	afx_msg void OnUpdateFormatBringtofront(CCmdUI *pCmdUI);
	afx_msg void OnFormatSendtoback();
	afx_msg void OnUpdateFormatSendtoback(CCmdUI *pCmdUI);
	afx_msg void OnEditGroup();
	afx_msg void OnUpdateEditGroup(CCmdUI *pCmdUI);
	afx_msg void OnEditBreakapart();
	afx_msg void OnUpdateEditBreakapart(CCmdUI *pCmdUI);
	afx_msg void OnFormatBringforward();
	afx_msg void OnUpdateFormatBringforward(CCmdUI *pCmdUI);
	afx_msg void OnFormatSendbackward();
	afx_msg void OnUpdateFormatSendbackward(CCmdUI *pCmdUI);	
	afx_msg void OnEditUngroup();
	afx_msg void OnUpdateEditUngroup(CCmdUI *pCmdUI);
	afx_msg void OnExportResource();
	afx_msg void OnUpdateExportResource(CCmdUI *pCmdUI);
	
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnInsertText();
	afx_msg void OnUpdateInsertText(CCmdUI *pCmdUI);

protected:
	virtual BOOL SaveModified();
	virtual BOOL DoFileSave();
public:
	BOOL m_bSilentMode;
	afx_msg void OnGridShowgrid();
	afx_msg void OnUpdateGridShowgrid(CCmdUI *pCmdUI);
	afx_msg void OnGridSnaptogrid();
	afx_msg void OnUpdateGridSnaptogrid(CCmdUI *pCmdUI);
	afx_msg void OnGridEditgrid();
	afx_msg void OnFilePublish();
	afx_msg void OnFileFastExportMovie();
	afx_msg void OnInsertSound();
	afx_msg void OnUpdateInsertSound(CCmdUI *pCmdUI);
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI *pCmdUI);
	afx_msg void OnInsertImage();
	afx_msg void OnUpdateInsertImage(CCmdUI *pCmdUI);

	//afx_msg void OnTestCom();

protected:
	gldMainMovie2 *m_pMainMovie;
	CObjectMap *m_pObjectMap;

public:
	BOOL InitDocument(BOOL bCreateScene);	

public:
	inline gldMainMovie2 *GetMainMovie() {return m_pMainMovie;}
	inline CObjectMap *GetObjectMap() {return m_pObjectMap;}
	afx_msg void OnFileNewFromTemplate();
	afx_msg void OnEditMovieProperties();
	afx_msg void OnEditSceneProperties();
	afx_msg void OnUpdateEditSceneProperties(CCmdUI *pCmdUI);
};

extern CGlandaDoc *theDoc;
inline CGlandaDoc *_GetCurDocument()
{
	ASSERT(theDoc);
	return theDoc;
};

inline gldObj *_GetCurObj()
{
	return gldDataKeeper::Instance()->m_curObj;
}

inline gldMainMovie2 *_GetMainMovie2()
{
	return _GetCurDocument()->GetMainMovie();
}

inline CObjectMap *_GetObjectMap()
{
	return _GetCurDocument()->GetObjectMap();
}

inline gldScene2 *_GetCurScene2()
{
	gldMainMovie2 *pMainMovie = _GetMainMovie2();
	if (pMainMovie)
		return pMainMovie->GetCurScene();
	return NULL;
}

gldEffect *_GetCurEffect();

gldInstance *_GetCurInstance();
