// GlandaDoc.cpp : implementation of the CGlandaDoc class
//

#include "stdafx.h"
#include "Glanda.h"
#include "GlandaDoc.h"
#include "MainFrm.h"

#include "Global.h"
#include "DlgPreview.h"

#include "Graphics.h"
#include "FilePath.h"

#include "gld_archive.h"
#include "gld_func.h"
#include "my_app.h"
#include "toolsdef.h"

#include "SWFParse.h"

#include "gldSWFImporter.h"
#include "gldMovieClip.h"
#include "gldLayer.h"
#include "gldFrameClip.h"
#include "gldObj.h"
#include "gldLibrary.h"
#include "gldSWFEngine.h"

#include "gldShape.h"
#include "gldSprite.h"
#include "gldButton.h"
#include "gldText.h"
#include "gld_selection.h"

#include "TransAdaptor.h"

#include "Observer.h"

#include "DesignWnd.h"
#include "Typeset.h"

#include "BinStream.h"
#include "DrawHelper.h"

#include "CmdBringToFront.h"
#include "CmdSendToBack.h"
#include "CmdGroup.h"
#include "CmdBreakApart.h"

#include "PropGrid.h"

#include "jpeg.h"
#include "bitmap.h"
#include "gldImage.h"

#include "FileDialogEx.h"

#include "DlgProgress.h"
#include "DlgExport.h"

#include "SheetPublish.h"
#include "Options.h"

#include "ASView.h"
#include "ConvertToSWF.h"

#include "IniFile.h"
#include "JSFL.h"
#include "Registry.h"

#include "gldMainMovie2.h"

#include "SceneListCtrl.h"
#include "DlgTemplates.h"
#include "SWFStream.h"
#include "GlandaClipboard.h"
#include "gldObj.h"

#include "..\CommonDef\CommonDef.h"

#include "I2C.h"
#include "C2I.h"
#include "MoviePropertiesSheet.h"

#include "EditMCReplaceDialog.h"
#include "gldSound.h"
#include "CmdAddSound.h"

#include "ScenePropertiesSheet.h"
#include "EffectWnd.h"
#include "ImportImageDialog.h"
#include "gld_shape_builder.h"
#include "CmdModifyFillStyle.h"

#include "GlandaCommand.h"

#include "FLASymbolCreator.h"
#include "FLAFile.h"
#include "ComUtils.h"
#include "SWFStream.h"

#include "CategoryWnd.h"
#include "ProgressStatusBar.h"
#include "DlgInsertText.h"
#include "GlandaVersion.h"

#define SECTION_LINE_STYLE 1
#define SECTION_FILL_STYLE 2

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

gldEffect *_GetCurEffect()
{
	if (CEffectWnd::Instance()->GetSelType()==CEffectWnd::SEL_EFFECT)
	{
		return CEffectWnd::Instance()->GetSelEffect();
	}
	return NULL;
}

gldInstance *_GetCurInstance()
{
	if (my_app.CurSel().count() > 0)	
		return CTraitInstance(*my_app.CurSel().begin());
	else
		return NULL;
}

/*
BOOL CheckJSFLAssociation(BOOL bReport)
{
	CRegistry reg;
	if (reg.Open(HKEY_CLASSES_ROOT, ".jsfl", KEY_READ))
	{
		CString strName;
		if (reg.Read("", strName) && !strName.IsEmpty())
		{
			reg.Close();
			if (reg.Open(HKEY_CLASSES_ROOT, strName + "\\Shell\\Open\\Command", KEY_READ))
			{
				CString strCommand;
				if (reg.Read("", strCommand))
				{
					LPTSTR pszFlash = strCommand.GetBuffer();
					::PathRemoveArgs(pszFlash);
					strCommand.ReleaseBuffer();
				#ifdef _DEBUG
					CString strFlash;
					strFlash.Append(strCommand);
				#endif

					::PathStripPath(pszFlash);
					strCommand.ReleaseBuffer();
					if (strCommand.CompareNoCase("Flash.exe") == 0)
					{
					#ifdef _DEBUG
						AfxMessageBoxEx(MB_ICONINFORMATION | MB_OK, "Debug:\r\nFlash was found at %s", (LPCTSTR)strFlash);
					#endif
						return TRUE;
					}
				}
			}
		}
	}

	if (bReport)
		AfxMessageBox("Cannot launch Flash MX 2004 (or higher version). To use this feature, you must have Flash MX 2004 (or higher version) installed.", MB_ICONERROR | MB_OK);

	return FALSE;
}
*/

CGlandaDoc *theDoc = NULL;

void LeaveEditControl()
{
	CWnd *pWnd = CWnd::GetFocus();
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CSmartEdit)))
			((CSmartEdit *)pWnd)->HandleModified();
	}
}

// CGlandaDoc

IMPLEMENT_DYNCREATE(CGlandaDoc, CDocument)

BEGIN_MESSAGE_MAP(CGlandaDoc, CDocument)
	ON_COMMAND(ID_FILE_EXPORT_MOVIE, OnFileExportMovie)
	ON_COMMAND(ID_FILE_NEW_WINDOW, OnNewWindow)
	ON_COMMAND(ID_FILE_PREVIEW, OnFilePreview)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_FORMAT_ALIGNLEFTS, OnFormatAlighlefts)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNLEFTS, OnUpdateFormatAlighlefts)
	ON_COMMAND(ID_FORMAT_ALIGNTOPS, OnFormatAlightops)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNTOPS, OnUpdateFormatAlightops)
	ON_COMMAND(ID_FORMAT_ALIGNRIGHTS, OnFormatAlighrights)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNRIGHTS, OnUpdateFormatAlighrights)
	ON_COMMAND(ID_FORMAT_ALIGNBOTTOMS, OnFormatAlighbottoms)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNBOTTOMS, OnUpdateFormatAlighbottoms)
	ON_COMMAND(ID_FORMAT_ALIGNCENTERS, OnFormatAlighcenters)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNCENTERS, OnUpdateFormatAlighcenters)
	ON_COMMAND(ID_FORMAT_ALIGNMIDDLES, OnFormatAlighmiddles)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_ALIGNMIDDLES, OnUpdateFormatAlighmiddles)
	ON_COMMAND(ID_FORMAT_MAKESAMEWIDTH, OnFormatMakesamewidth)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKESAMEWIDTH, OnUpdateFormatMakesamewidth)
	ON_COMMAND(ID_FORMAT_MAKESAMEHEIGHT, OnFormatMakesameheight)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKESAMEHEIGHT, OnUpdateFormatMakesameheight)
	ON_COMMAND(ID_FORMAT_MAKESAMESIZE, OnFormatMakesamesize)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKESAMESIZE, OnUpdateFormatMakesamesize)
	ON_COMMAND(ID_FORMAT_MAKEHORIZONTALSPACINGEQUAL, OnFormatMakehorizontalspacingequal)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKEHORIZONTALSPACINGEQUAL, OnUpdateFormatMakehorizontalspacingequal)
	ON_COMMAND(ID_FORMAT_MAKEVERTICALSPACINGEQUAL, OnFormatMakeverticalspacingequal)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_MAKEVERTICALSPACINGEQUAL, OnUpdateFormatMakeverticalspacingequal)
	ON_COMMAND(ID_FORMAT_BRINGTOFRONT, OnFormatBringtofront)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_BRINGTOFRONT, OnUpdateFormatBringtofront)
	ON_COMMAND(ID_FORMAT_SENDTOBACK, OnFormatSendtoback)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_SENDTOBACK, OnUpdateFormatSendtoback)
	ON_COMMAND(ID_EDIT_GROUP, OnEditGroup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GROUP, OnUpdateEditGroup)
	ON_COMMAND(ID_EDIT_BREAKAPART, OnEditBreakapart)
	ON_UPDATE_COMMAND_UI(ID_EDIT_BREAKAPART, OnUpdateEditBreakapart)
	ON_COMMAND(ID_FORMAT_BRINGFORWARD, OnFormatBringforward)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_BRINGFORWARD, OnUpdateFormatBringforward)
	ON_COMMAND(ID_FORMAT_SENDBACKWARD, OnFormatSendbackward)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_SENDBACKWARD, OnUpdateFormatSendbackward)	
	ON_COMMAND(ID_EDIT_UNGROUP, OnEditUngroup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNGROUP, OnUpdateEditUngroup)
	ON_COMMAND(ID_GRID_SHOWGRID, OnGridShowgrid)
	ON_UPDATE_COMMAND_UI(ID_GRID_SHOWGRID, OnUpdateGridShowgrid)
	ON_COMMAND(ID_GRID_SNAPTOGRID, OnGridSnaptogrid)
	ON_UPDATE_COMMAND_UI(ID_GRID_SNAPTOGRID, OnUpdateGridSnaptogrid)
	ON_COMMAND(ID_GRID_EDITGRID, OnGridEditgrid)
	ON_COMMAND(ID_FILE_PUBLISH, OnFilePublish)
	ON_COMMAND(ID_FILE_FAST_EXPORT_MOVIE, OnFileFastExportMovie)
	ON_COMMAND(ID_FILE_NEW_FROM_TEMPLATE, OnFileNewFromTemplate)

	//ON_COMMAND(ID_TEST_COMTEST, OnTestCom)
	ON_COMMAND(ID_EDIT_MOVIE_PROPERTIES, OnEditMovieProperties)

	ON_COMMAND(ID_FILE_EXPORTRESOURCE, OnExportResource)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTRESOURCE, OnUpdateExportResource)

	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)

	ON_COMMAND(ID_INSERT_SOUND, OnInsertSound)
	ON_UPDATE_COMMAND_UI(ID_INSERT_SOUND, OnUpdateInsertSound)

	ON_COMMAND(ID_INSERT_IMAGE, OnInsertImage)
	ON_UPDATE_COMMAND_UI(ID_INSERT_IMAGE, OnUpdateInsertImage)

	ON_COMMAND(ID_EDIT_SCENE_PROPERTIES, OnEditSceneProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SCENE_PROPERTIES, OnUpdateEditSceneProperties)
	ON_COMMAND(ID_INSERT_TEXT, OnInsertText)
	ON_UPDATE_COMMAND_UI(ID_INSERT_TEXT, OnUpdateInsertText)
END_MESSAGE_MAP()


// CGlandaDoc construction/destruction
CGlandaDoc::CGlandaDoc()
{
	theDoc = this;
	m_bSilentMode = FALSE;
	m_pMainMovie = NULL;
}

CGlandaDoc::~CGlandaDoc()
{
}

static void LoadStylesConfig()
{	
	if (!theIniFile->Ready())
		return;

	CIniSection LsSec(SECTION_LINE_STYLE);
	CIniSection FsSec(SECTION_FILL_STYLE);

	// load line style from ini file	
	if (LsSec.LoadFromFile(theIniFile))
	{
		TLineStyle *pls = NULL;

		if (LsSec.Size() > 0)
		{
			int width = LsSec.Read((int)1);
			long color = LsSec.Read((long)MAKECOLOR(255, 0, 0, 255));			
			pls = new TLineStyle(width, TColor(color));
		}

		my_app.SetCurLineStyle(pls);
	}
	
	// load fill style from ini file
	if (FsSec.LoadFromFile(theIniFile))
	{
		TFillStyle *pfs = NULL;

		if (FsSec.Size() > 0)
		{			
			int type = 0;
			type = FsSec.Read(type);
			if (type == TFillStyle::solid_fill)
			{
				long color = FsSec.Read((long)MAKECOLOR(255, 0, 0, 255));
				pfs = new TSolidFillStyle(color);
			}
			else if (type == TFillStyle::linear_gradient_fill
				|| type == TFillStyle::radial_gradient_fill)
			{
				if (type == TFillStyle::linear_gradient_fill)
					pfs = new TLinearGradientFillStyle;
				else
					pfs = new TRadialGradientFillStyle;	

				int count = FsSec.Read((int)0);
				count = __min(__max(0, count), 255);
				for (int i = 0; i < count; i++)
				{
					U8 ratio = FsSec.Read((U8)0);
					long color = FsSec.Read((long)MAKECOLOR(255, 0, 0, 255));
					((TGradientFillStyle *)pfs)->AddRecord(color, ratio);
				}
			}			
			else			
				ASSERT(FALSE);
		}

		my_app.SetCurFillStyle(pfs);
	}
}

BOOL CGlandaDoc::InitDocument(BOOL bCreateScene)
{
	if (!CDocument::OnNewDocument())
		return FALSE;	
	
	ASSERT(m_pMainMovie == NULL);
	m_pMainMovie = new gldMainMovie2();

	CWinApp *pApp = AfxGetApp();

	CString strFrameRate = pApp->GetProfileString("Movie", "Frame Rate", "24");
	m_pMainMovie->m_frameRate = max(0.01f, min(120, (float)atof(strFrameRate)));
	m_pMainMovie->m_width = max(1, min(2880, pApp->GetProfileInt("Movie", "Width", 400)));
	m_pMainMovie->m_height = max(1, min(2880, pApp->GetProfileInt("Movie", "Height", 300)));
	m_pMainMovie->m_color = 0xffffff & pApp->GetProfileInt("Movie", "Background", 0xffffff);

	//m_pLibrary = new gldLibrary();
	m_pObjectMap = new CObjectMap();
    //CTransAdaptor::SetAdaptor(m_pLibrary);

	my_app.Create();	
	//LoadStylesConfig();

	CGuardDrawOnce	xDraw;
	CDesignWnd::Instance()->Reset();

	my_app.ShowCanvasBorder(COptions::Instance()->m_bShowCanvasBorder ? true: false);
	my_app.SetMovieSize(CSize(m_pMainMovie->m_width, m_pMainMovie->m_height), false);
	my_app.SetMovieBackground(m_pMainMovie->m_color, false);
	my_app.SetCurTool(IDT_TRANSFORM);
	my_app.CenterMovie();	

	my_app.SetCurTool(IDT_TRANSFORM);
	my_app.CenterMovie();

	m_strTitle.LoadString(AFX_IDS_UNTITLED);

	if (bCreateScene)
	{
		CString strScene;
		strScene.LoadString(IDS_KEY_SCENE);
		strScene += " 1";
		gldScene2 *pScene = new gldScene2();
		pScene->m_name = (LPCTSTR)strScene;
		m_pMainMovie->m_sceneList.push_back(pScene);
		m_pMainMovie->SetCurScene(pScene, FALSE);

		CTransAdaptor::BuildCurrentScene(false);
	}

	CSubjectManager::Instance()->GetSubject("ChangeMainMovie2")->Notify(0);

	return TRUE;
}

BOOL CGlandaDoc::OnNewDocument()
{
	return InitDocument(TRUE);
}

int external_CheckExportFile(const char *filename)
{
    return 1;
}

// CGlandaDoc commands
BOOL CGlandaDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	#define _CHECKRESULT()	if (FAILED(hr))	throw (hr);

	BOOL ret = TRUE;

	CProgressStatusBar::Instance()->BeginProgress();

	gldLibrary *pOldLib = NULL;
	try
	{
		HRESULT hr;

		USES_CONVERSION;

		CComPtr<IStorage> pStg;
		hr = StgOpenStorage(A2OLE(lpszPathName), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStg);
		_CHECKRESULT();


		// read version info
		CComPtr<IStorage> pStgDoc;
		hr = pStg->OpenStorage(L"Doc", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgDoc);
		_CHECKRESULT();

		// read magic
		CComPtr<IStream> pStmMagic;
		hr = pStgDoc->OpenStream(L"Magic", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmMagic);
		_CHECKRESULT();

		IStreamWrapper stmMagic(pStmMagic);
		WORD wMagic = 0;
		hr = stmMagic.Read(wMagic);
		_CHECKRESULT();

		// read version
		CComPtr<IStream> pStmVersion;
		hr = pStgDoc->OpenStream(L"Version", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmVersion);
		_CHECKRESULT();

		IStreamWrapper stmVersion(pStmVersion);
		WORD wVersion = 0;
		hr = stmVersion.Read(wVersion);
		_CHECKRESULT();

		// check magiuc and version
		if (wMagic != MAKEWORD('G', 'L') || wVersion == 0)
		{
			AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_INVALID_DOCUMENT_1, lpszPathName);
			AfxThrowUserException();
		}

		if (!CGlandaVersion::IsValidVersion(wVersion))
		{
			AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_DOCUMENT_VERSION_HIGHER_1, lpszPathName);
			AfxThrowUserException();
		}
		
		//向GLANDA COM设置当前库的正确版本。
		WORD swflibVer = CGlandaVersion::GetSWFLibVersion(wVersion);
		gldDataKeeper::Instance()->m_curDocMajorVersion = CGlandaVersion::GetSWFLibMajor(swflibVer);
		gldDataKeeper::Instance()->m_curDocMinorVersion = CGlandaVersion::GetSWFLibMinor(swflibVer);

		// read library
		CComPtr<IStorage> pStgLib;
		hr = pStg->OpenStorage(L"Lib", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgLib);
		_CHECKRESULT();

		CComPtr<IStream> pStmIndex;
		hr = pStgLib->OpenStream(L"Index", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStmIndex);
		_CHECKRESULT();

		IStreamWrapper stmIndex(pStmIndex);
		WORD verIndex = 0;
		hr = stmIndex.Read(verIndex);
		_CHECKRESULT();

		vector<string> vecSymbols;
		if (verIndex == 1)
		{
			DWORD count = 0;
			hr = stmIndex.Read(count);
			_CHECKRESULT();

			for (; count > 0; count--)
			{
				string name;
				hr = stmIndex.Read(name);
				_CHECKRESULT();

				vecSymbols.push_back(name);
			}
		}
		else
		{
			throw(E_FAIL);
		}
		pStmIndex.Release();

		gldLibrary lib;	
		CTransAdaptor::SetAdaptor(&lib);
		pOldLib = gldDataKeeper::Instance()->m_objLib;
		gldDataKeeper::Instance()->m_objLib = &lib;
		vector<string>::iterator itn = vecSymbols.begin();
		
		for (int nSymbols = 0; itn != vecSymbols.end(); ++itn, ++nSymbols)
		{
			CComPtr<IStream> pStm;
			hr = pStgLib->OpenStream(A2OLE((*itn).c_str()), 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStm);
			_CHECKRESULT();

			IStreamWrapper stm(pStm);
			WORD objType = 0;
			hr = stm.Read(objType);
			_CHECKRESULT();

			iBinStream is;
			hr = ReadFromStream(pStm, is);
			_CHECKRESULT();

			gldObj *pObj = ::CreateObject((GObjType)objType);
			if (pObj == NULL)
				AfxThrowMemoryException();

			pObj->ReadFromBinStream(is);
			ASSERT(pObj->m_ptr == NULL);
			CSWFProxy::PrepareTShapeForGObj(pObj);
			lib.AddObj(pObj, false);

			CProgressStatusBar::Instance()->SetPos((nSymbols + 1) * 70 / vecSymbols.size());
		}
		
		CComPtr<IStorage> pStgMain;
		hr = pStg->OpenStorage(L"Main", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgMain);
		_CHECKRESULT();

		CAutoPtr<gldMainMovie2> mainMovie(new gldMainMovie2);
		hr = mainMovie->Load(pStgMain, (void *)&lib);
		_CHECKRESULT();

		gldDataKeeper::Instance()->m_objLib = pOldLib;
		
		DeleteContents();
	    
		// copy library to ObjectMap
		// to do ...
		const GOBJECT_LIST &lstObj = lib.GetObjList();
		GOBJECT_LIST::const_iterator ito = lstObj.begin();
		m_pObjectMap = new CObjectMap;
		for (; ito != lstObj.end(); ++ito)
		{
			m_pObjectMap->UpdateUniId(*ito);
			_GetObjectMap()->Use(*ito);
		}
		lib.ClearList();

		my_app.Create();
		//LoadStylesConfig();

		CGuardDrawOnce	xDraw;
		CDesignWnd::Instance()->Reset();

		my_app.ShowCanvasBorder(COptions::Instance()->m_bShowCanvasBorder ? true: false);
		my_app.SetMovieSize(CSize(mainMovie->m_width, mainMovie->m_height), false);
		my_app.SetMovieBackground(mainMovie->m_color, false);
		my_app.SetCurTool(IDT_TRANSFORM);
		my_app.CenterMovie();

		m_pMainMovie = mainMovie.Detach();

		CTransAdaptor::RebuildCurrentScene(false);
		CSubjectManager::Instance()->GetSubject("ChangeMainMovie2")->Notify(0);
		CSubjectManager::Instance()->GetSubject("ChangeCurrentScene2")->Notify(0);
		CSubjectManager::Instance()->GetSubject("Select")->Notify(0);

		m_strPathName = lpszPathName;
		m_strTitle = m_strPathName;
		::PathStripPath(m_strTitle.GetBuffer(0));
		m_strTitle.ReleaseBuffer();
	}
	catch (HRESULT hr)
	{
		hr;

		if (pOldLib)
			gldDataKeeper::Instance()->m_objLib = pOldLib;
		
		//switch (hr)
		//{
		//	case STG_E_FILENOTFOUND:		//%1 could not be found
		//	case STG_E_FILEALREADYEXISTS:	// %1 already exists
		//		AfxMessageBox("Invalid file format.", MB_ICONERROR | MB_OK);
		//		break;

		//	default:
		//		::CoReportError(hr);
		//		break;
		//}
	#ifdef _DEBUG
		::CoReportError(hr);
	#endif

		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_FAILED_OPEN_FILE_1, lpszPathName);

		ret =  FALSE;
	}
	catch (CUserException *e)
	{
		if (pOldLib)
			gldDataKeeper::Instance()->m_objLib = pOldLib;

		// error is already processed
		e->Delete();
		ret = FALSE;
	}
	catch (CException *e)
	{
		if (pOldLib)
			gldDataKeeper::Instance()->m_objLib = pOldLib;

		e->ReportError();
		e->Delete();

		ret = FALSE;
	}

	CProgressStatusBar::Instance()->EndProgress();

	// set cswflib document version
	WORD swflibVer = CGlandaVersion::GetSWFLibVersion(CGlandaVersion::GetCurVersion());
	gldDataKeeper::Instance()->m_curDocMajorVersion = CGlandaVersion::GetSWFLibMajor(swflibVer);
	gldDataKeeper::Instance()->m_curDocMinorVersion = CGlandaVersion::GetSWFLibMinor(swflibVer);

	return ret;
}

#define STGM_RWEC (STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)

BOOL CGlandaDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	BOOL ret = TRUE;

	CProgressStatusBar::Instance()->BeginProgress();

	LeaveEditControl();

	my_app.CurTool()->Leave();

	BOOL bUseTempFile = TRUE;
	CString strTempFile = GetTempFile(NULL, "~glf", "tmp");
	if (strTempFile.IsEmpty())
	{
	#ifdef _DEBUG
		AfxMessageBox("Failed to create temp file.", MB_ICONERROR | MB_OK);
	#endif
		bUseTempFile = FALSE;
	}

	#define _CHECKRESULT()	if (FAILED(hr))	throw (hr);
	try
	{
		USES_CONVERSION;
		CComPtr<IStorage> pStg;
		HRESULT hr = StgCreateDocfile(A2OLE(bUseTempFile ? strTempFile : lpszPathName),
			STGM_RWEC, 0, &pStg);
		_CHECKRESULT();

		// save document info to "Doc" storage
		CComPtr<IStorage> pStgDoc;
		hr = pStg->CreateStorage(L"Doc", STGM_RWEC, 0, 0, &pStgDoc);
		_CHECKRESULT();

		// write magic
		CComPtr<IStream> pStmMagic;
		hr = pStgDoc->CreateStream(L"Magic", STGM_RWEC, 0, 0, &pStmMagic);
		_CHECKRESULT();

		IStreamWrapper stmMagic(pStmMagic);
		WORD wMagic = MAKEWORD('G', 'L');
		hr = stmMagic.Write(wMagic);
		_CHECKRESULT();

		// write version
		CComPtr<IStream> pStmVersion;
		hr = pStgDoc->CreateStream(L"Version", STGM_RWEC, 0, 0, &pStmVersion);
		_CHECKRESULT();

		IStreamWrapper stmVersion(pStmVersion);
		WORD wVersion = CGlandaVersion::GetCurVersion();
		hr = stmVersion.Write(wVersion);
		_CHECKRESULT();


		// save object library to "Lib" storage
		CComPtr<IStorage> pStgLib;
		hr = pStg->CreateStorage(L"Lib", STGM_RWEC, 0, 0, &pStgLib);
		_CHECKRESULT();

		// only save used object
		GOBJECT_LIST lstObj;
		gldMainMovie2 *mainMovie = _GetMainMovie2();
		ASSERT(mainMovie != NULL);
		mainMovie->GetUsedObjs(lstObj);
		SortObjsByRef(lstObj);
		int index = 1;
		GOBJECT_LIST_IT ito = lstObj.begin();
		for (; ito != lstObj.end(); ++ito)
		{
			(*ito)->m_id = index++;
		}

		// create index table in "Lib" directory
		CComPtr<IStream> pStmIndex;
		hr = pStgLib->CreateStream(L"Index", STGM_RWEC, 0, 0, &pStmIndex);
		_CHECKRESULT();

		IStreamWrapper stmIndex(pStmIndex);	
		WORD verIndex = 1;	// index table version
		hr = stmIndex.Write(verIndex);
		_CHECKRESULT();

		hr = stmIndex.Write((DWORD)lstObj.size());
		_CHECKRESULT();

		for (ito = lstObj.begin(); ito != lstObj.end(); ++ito)
		{
			gldObj *pObj = *ito;
			char stmName[20];
			sprintf(stmName, "S%d", pObj->m_id);		
			hr = stmIndex.Write(string(stmName));
			_CHECKRESULT();
		}
		pStmIndex.Release();

		// write each object to "Lib" storage
		int nSymbols = 0;
		for (ito = lstObj.begin(); ito != lstObj.end(); ++ito, ++nSymbols)
		{
			gldObj *pObj = *ito;		
			OLECHAR stmName[20];
			swprintf(stmName, L"S%d", pObj->m_id);
			CComPtr<IStream> pStm;
			hr = pStgLib->CreateStream(stmName, STGM_RWEC, 0, 0, &pStm);
			_CHECKRESULT();

			oBinStream os;
			pObj->WriteToBinStream(os);
			IStreamWrapper stm(pStm);
			hr = stm.Write((WORD)pObj->GetGObjType());	// object type
			_CHECKRESULT();

			hr = WriteToStream(os, pStm);
			_CHECKRESULT();

			CProgressStatusBar::Instance()->SetPos((nSymbols + 1) * 70 / lstObj.size());
		}

		// save main movie to "Main" storage
		CComPtr<IStorage> pStgMain;
		hr = pStg->CreateStorage(L"Main", STGM_RWEC, 0, 0, &pStgMain);
		_CHECKRESULT();

		hr = mainMovie->Save(pStgMain, NULL);
		_CHECKRESULT();
		
		pStg.Release(); // close doc file
	}
	catch (HRESULT hr)
	{
	#ifdef _DEBUG
		CoReportError(hr);
	#endif

		hr;
		::DeleteFile(bUseTempFile ? strTempFile : lpszPathName);
		AfxMessageBox(IDS_FAILED_SAVE_DOCUMENT, MB_ICONERROR | MB_OK);
		ret = FALSE;
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();

		::DeleteFile(bUseTempFile ? strTempFile : lpszPathName);
		ret = FALSE;
	}

	if (ret && bUseTempFile)
	{
		::DeleteFile(lpszPathName);

		// MoveFile may fail on systems with Anti-Virus software installed.
		int i = 0;
		for ( i = 0; i < 4; i++)
		{
			if (::MoveFile(strTempFile, lpszPathName))
				break;

			TRACE("MoveFile(%s, %s) was failed.\n", strTempFile, lpszPathName);
			Sleep(1);
		}

		::DeleteFile(strTempFile);
		if (i == 4)
		{
			AfxMessageBoxEx(MB_OK | MB_ICONERROR, IDS_ERROR_SAVE_FILE_PERMISSION1, lpszPathName);
			ret = FALSE;
		}
	}

	if (ret)
	{
		SetModifiedFlag(FALSE);
		my_app.Commands().SetSavePoint();
	}

	CProgressStatusBar::Instance()->EndProgress();

	return ret;
}

static void SaveStylesConfig()
{	
	if (!theIniFile->Ready())
		return;

	// save current line style to ini file
	TLineStyle *pls = my_app.GetCurLineStyle();
	if (pls == NULL)
		theIniFile->WriteSection(SECTION_LINE_STYLE, NULL, 0);
	else
	{
		CIniSection LsSec(SECTION_LINE_STYLE);
		LsSec.Write(pls->GetWidth());
		LsSec.Write(pls->GetColor().GetColor());
		LsSec.SaveToFile(theIniFile);
	}

	// save current fill style to ini file
	TFillStyle *pfs = my_app.GetCurFillStyle();
	if (pfs == NULL)
		theIniFile->WriteSection(SECTION_FILL_STYLE, NULL, 0);
	else
	{
		CIniSection FsSec(SECTION_FILL_STYLE);

		int type = pfs->GetType();		
		if (type == TFillStyle::solid_fill)
		{	
			FsSec.Write(type);
			FsSec.Write(((TSolidFillStyle *)pfs)->GetColor().GetColor());
		}
		else if (pfs->GetType() == TFillStyle::linear_gradient_fill 
			|| pfs->GetType() == TFillStyle::radial_gradient_fill)
		{			
			FsSec.Write(type);

			LPGGRADIENTRECORD pRec = ((TGradientFillStyle *)pfs)->GetFirstRecord();
			int count = 0;
			while (pRec != NULL) 
			{
				count++;
				pRec = pRec->next;
			}
			FsSec.Write(count);

			pRec = ((TGradientFillStyle *)pfs)->GetFirstRecord();			
			while (pRec != NULL)
			{				
				FsSec.Write(pRec->ratio);				
				FsSec.Write(pRec->color);
				pRec = pRec->next;
			}
		}		
		else
		{	// default fill style
			type = TFillStyle::solid_fill;
			TColor color(255, 0, 0, 255);
			FsSec.Write(type);
			FsSec.Write(color.GetColor());
		}
		
		FsSec.SaveToFile(theIniFile);
	}
}

void CGlandaDoc::DeleteContents()
{
	if (my_app.Ready())
	{
		// the last DeleteContents() is called after all the windows have been destroyed
		if (AfxGetMainWnd() != NULL)
		{
			CSceneListCtrl::Instance()->DeleteAllItems();
		}

		// must deselect all shapes, or SaveStylesConfig() will 
		// save the fill style of the selected shape.
		my_app.DoSelect(gld_shape_sel());
		//SaveStylesConfig();

		my_app.Destroy();
		gldDataKeeper::ReleaseInstance();

		delete m_pMainMovie;
		m_pMainMovie = NULL;

		//delete m_pLibrary;
		//m_pLibrary = NULL;
		delete m_pObjectMap;
		m_pObjectMap = NULL;
	}

	CDocument::DeleteContents();

    gldDataKeeper::m_docId++;

	m_strPathName.Empty();
	m_strTitle.Empty();

	m_strMovieExport.Empty();
}

void CGlandaDoc::OnCloseDocument()
{
	// must call before CDocument::DeleteContents() call.
	my_app.DoSelect(gld_shape_sel());

	AfxGetApp()->WriteProfileString("Last File", "Last File", GetPathName());
	CDocument::OnCloseDocument();
}

// CGlandaDoc diagnostics

#ifdef _DEBUG
void CGlandaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGlandaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

void CGlandaDoc::SetModifiedFlag(BOOL bModified)
{
	if (bModified != CDocument::IsModified())
	{
		CDocument::SetModifiedFlag(bModified);
		((CFrameWnd *)AfxGetMainWnd())->OnUpdateFrameTitle(TRUE);
	}
}

void CGlandaDoc::OnFilePreview()
{
	// TODO: Add your command handler code here

	CDlgPreview *pDlg = CDlgPreview::Instance();
	if (::IsWindow(pDlg->m_hWnd))
		pDlg->DestroyWindow();

	LeaveEditControl();

	my_app.CurTool()->Leave();

	CString strFile = GetTempFile(COptions::Instance()->GetPreviewPath(), "~preview", "swf");
	if (ExportMovie(AfxGetMainWnd(), strFile, FALSE, TRUE))
	{
		pDlg->m_strFile = strFile;
		pDlg->Create(IDD_PREVIEW, CWnd::GetDesktopWindow());
		pDlg->ShowWindow(SW_SHOW);
	}
}

void CGlandaDoc::OnFileExportMovie()
{
	LeaveEditControl();

	my_app.CurTool()->Leave();

	CString strName = GetExportMovieName();

	CString strTitle, strFilter;
	strTitle.LoadString(IDS_EXPORT_MOVIE_TITLE);
	strFilter.LoadString(IDS_FILTER_SWF);
	CDlgExport dlg(strTitle, FALSE, "swf", strFilter, strName);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if (dlg.DoModal() == IDOK)
	{
		m_strMovieExport = dlg.GetPathName();
        ExportMovie(AfxGetMainWnd(), m_strMovieExport, dlg.m_bCompress, FALSE);
	}
}

void CGlandaDoc::OnFileFastExportMovie()
{
	// TODO: Add your command handler code here

	LeaveEditControl();

	my_app.CurTool()->Leave();

	if (m_strMovieExport.IsEmpty())
	{
		AfxMessageBox(IDS_PROMPT_FAST_EXPORT);
		return;
	}
	CString strName = GetExportMovieName();
	ExportMovie(AfxGetMainWnd(), m_strMovieExport, AfxGetApp()->GetProfileInt("Export", "Compress", TRUE), FALSE);
}

void CGlandaDoc::OnFilePublish()
{
	// TODO: Add your command handler code here

	LeaveEditControl();

	my_app.CurTool()->Leave();

	CSheetPublish dlg(IDS_PUBLISH);
	dlg.DoModal();
}

void CGlandaDoc::OnNewWindow()
{
	TCHAR szName[MAX_PATH + 1];
	if (::GetModuleFileName(NULL, szName, _MAX_PATH))
		::ShellExecute(NULL, "open", szName, "-newwin", NULL, SW_SHOWNORMAL);
}

void CGlandaDoc::OnFileImport()
{
	DoFileImport();
}

void CGlandaDoc::OnUpdateFileImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

void CGlandaDoc::DoFileImport()
{
	CString strTitle, strFilter;
	strTitle.LoadString(IDS_IMPORT_TITLE);
	strFilter.LoadString(IDS_FILTER_ALL);
	CFileDialogEx dlg(strTitle, TRUE, NULL, strFilter);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if (dlg.DoModal() == IDOK)
	{
		CSize size = my_app.GetViewSize();
		CPoint point(size.cx / 2, size.cy / 2);

        CString _fileName = dlg.GetPathName();
        CString _ext = PathFindExtension(_fileName);
        if (_ext.CompareNoCase(".swf") == 0 || _ext.CompareNoCase(".exe") == 0)
        {
			ImportSWF(_fileName);
		}
		else if (_ext.CompareNoCase(".bmp") == 0 
			|| _ext.CompareNoCase(".jpg") == 0 
			|| _ext.CompareNoCase(".jpe") == 0 
			|| _ext.CompareNoCase(".jpeg") == 0
			|| _ext.CompareNoCase(".png") == 0)
		{
			ImportImage(_fileName, point, TRUE);
		}
		else if (_ext.CompareNoCase(".wmf") == 0
			|| _ext.CompareNoCase(".emf") == 0
			|| _ext.CompareNoCase(".svg") == 0
			|| _ext.CompareNoCase(".ai") == 0)
		{
			ImportVectorFormat(_fileName, point);
		}
		else if (_ext.CompareNoCase(".gls") == 0)
		{
			ImportGLS(_fileName, point);
		}
		else if (_ext.CompareNoCase(".glm") == 0 || _ext.CompareNoCase(".glb") == 0)
		{
			ImportGLC(_fileName, point);
		}
		else if (_ext.CompareNoCase(".wav") == 0 || _ext.CompareNoCase(".mp3") == 0)
		{
			ImportSound(_fileName);
		}
		else
		{
			// TO DO: should determine the file type
			AfxMessageBox(IDS_ERROR_UNKNOWN_EXTENSION);
		}
	}
}

BOOL CGlandaDoc::ImportSound(LPCTSTR lpszFileName)
{
	LeaveEditControl();
	my_app.CurTool()->Leave();

	CWaitCursor wc;

	gldSound* pSound = new gldSound();
	if (!pSound->ReadFile(lpszFileName))
	{
		delete pSound;
		AfxMessageBox(IDS_ERROR_IMPORT);
		return FALSE;
	}
	pSound->m_name = FileStripPath(lpszFileName);
	//pSound->UpdateUniId();
	_GetObjectMap()->UpdateUniId(pSound);

	gldSceneSound *pSceneSound = new gldSceneSound();
	pSceneSound->m_sound->m_soundObj = pSound;
	pSceneSound->m_startTime = 0;
	pSceneSound->m_length = (int)max(1, pSound->GetSoundTime() * _GetMainMovie2()->m_frameRate);
	pSceneSound->m_name = pSound->m_name;

	gldScene2 *pScene = _GetCurScene2();
	if (pScene->m_soundList.size() > 0)
	{
		gldSceneSound *pCurSceneSound = *pScene->m_soundList.rbegin();
		pSceneSound->m_startTime = pCurSceneSound->m_startTime + pCurSceneSound->m_length;
	}

	TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_IMPORTSOUND);
	pCmdGroup->Do(new CCmdAddObj(pSound));
	pCmdGroup->Do(new CCmdAddSound(pSceneSound));
	return my_app.Commands().Do(pCmdGroup) ? TRUE : FALSE;
}

void CGlandaDoc::OnEditUndo()
{
	// TODO: Add your command handler code here
	
	if (my_app.Commands().CanUndo())
	{
		LeaveEditControl();

		my_app.CurTool()->Leave();
		
		my_app.Commands().Undo();
		((CMainFrame *)AfxGetMainWnd())->GetToolBar().GetToolBarCtrl().GetToolTips()->Update();
	}
}

void CGlandaDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->Enable(my_app.Commands().CanUndo());
}

void CGlandaDoc::OnEditRedo()
{
	// TODO: Add your command handler code here

	TRACE0("OnEditRedo()\n");

	if (my_app.Commands().CanRedo())
	{
		LeaveEditControl();

		my_app.CurTool()->Leave();

		my_app.Commands().Redo();
		((CMainFrame *)AfxGetMainWnd())->GetToolBar().GetToolBarCtrl().GetToolTips()->Update();
	}
}

void CGlandaDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->Enable(my_app.Commands().CanRedo());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Format Command
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CGlandaDoc::OnFormatAlighlefts()
{
	// TODO: Add your command handler code here	
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_LEFT);
}

void CGlandaDoc::OnUpdateFormatAlighlefts(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatAlightops()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_TOP);
}

void CGlandaDoc::OnUpdateFormatAlightops(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatAlighrights()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_RIGHT);
}

void CGlandaDoc::OnUpdateFormatAlighrights(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatAlighbottoms()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_BOTTOM);
}

void CGlandaDoc::OnUpdateFormatAlighbottoms(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatAlighcenters()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_CENTER);
}

void CGlandaDoc::OnUpdateFormatAlighcenters(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatAlighmiddles()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::ALIGN_MIDDLE);
}

void CGlandaDoc::OnUpdateFormatAlighmiddles(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatMakesamewidth()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::MAKE_SAME_WIDTH);
}

void CGlandaDoc::OnUpdateFormatMakesamewidth(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatMakesameheight()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::MAKE_SAME_HEIGHT);
}

void CGlandaDoc::OnUpdateFormatMakesameheight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatMakesamesize()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::MAKE_SAME_SIZE);
}

void CGlandaDoc::OnUpdateFormatMakesamesize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatMakehorizontalspacingequal()
{
	// TODO: Add your command handler code here
	CTypeset::Format(my_app.CurSel(), CTypeset::MAKE_HSPACING_EQUAL);
}

void CGlandaDoc::OnUpdateFormatMakehorizontalspacingequal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatMakeverticalspacingequal()
{
	CTypeset::Format(my_app.CurSel(), CTypeset::MAKE_VSPACING_EQUAL);
}

void CGlandaDoc::OnUpdateFormatMakeverticalspacingequal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnFormatBringtofront()
{
	// TODO: Add your command handler code here
	if (!my_app.CurSel().empty())
	{
		CCmdBringToFront	*pCmd = new CCmdBringToFront(my_app.CurSel(), 1);

		my_app.Commands().Do(pCmd, false);
	}
}

void CGlandaDoc::OnUpdateFormatBringtofront(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 0);
}

void CGlandaDoc::OnFormatSendtoback()
{
	// TODO: Add your command handler code here
	if (!my_app.CurSel().empty())
	{
		CCmdSendToBack	*pCmd = new CCmdSendToBack(my_app.CurSel(), 1);

		my_app.Commands().Do(pCmd, false);
	}
}

void CGlandaDoc::OnUpdateFormatSendtoback(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() > 0);
}

/*
void CGlandaDoc::OnConvertSymbol()
{
	// TODO: Add your command handler code here

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 0)
	{
		ASSERT(FALSE);
		return;
	}

	CDlgConvertSymbol dlg;
	dlg.DoModal();
}

void CGlandaDoc::OnUpdateConvertSymbol(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	gld_shape_sel sel = my_app.CurSel();
	
	pCmdUI->Enable(sel.count() > 0);
}
*/

void CGlandaDoc::OnEditGroup()
{
	// TODO: Add your command handler code here
	CCmdGroupInstance *pCmd = new CCmdGroupInstance(my_app.CurSel());

	my_app.Commands().Do(pCmd);
}

void CGlandaDoc::OnUpdateEditGroup(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 1);
}

void CGlandaDoc::OnEditBreakapart()
{
	// TODO: Add your command handler code here
	if (my_app.CurSel().count() == 1 && CCmdBreakApart::CanBreakApart(*my_app.CurSel().begin()))
	{		
		CCmdBreakApart	*pCmd = new CCmdBreakApart(*my_app.CurSel().begin());

		my_app.Commands().Do(pCmd);
	}
}

void CGlandaDoc::OnUpdateEditBreakapart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (my_app.CurSel().count() == 1)
	{
		pCmdUI->Enable(CCmdBreakApart::CanBreakApart(*my_app.CurSel().begin()));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CGlandaDoc::OnFormatBringforward()
{
	// TODO: Add your command handler code here
	if (!my_app.CurSel().empty())
	{
		CCmdBringToFront	*pCmd = new CCmdBringToFront(my_app.CurSel(), 0);

		my_app.Commands().Do(pCmd, false);
	}
}

void CGlandaDoc::OnUpdateFormatBringforward(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 0);
}

void CGlandaDoc::OnFormatSendbackward()
{
	// TODO: Add your command handler code here
	if (!my_app.CurSel().empty())
	{
		CCmdSendToBack	*pCmd = new CCmdSendToBack(my_app.CurSel(), 0);

		my_app.Commands().Do(pCmd, false);
	}
}

void CGlandaDoc::OnUpdateFormatSendbackward(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(my_app.CurSel().count() > 0);
}

IMPLEMENT_OBSERVER(CGlandaDoc, ModifyDocument)
{
	SetModifiedFlag((BOOL)pData);
}

//IMPLEMENT_OBSERVER(CGlandaDoc, Select)
//{
//	gld_shape shape = my_app.CurSel().Activate();
//	if (!shape.validate() && my_app.CurSel().count() > 0)
//		shape = *my_app.CurSel().begin();
//
//	if (shape.validate())
//		gldInstance *pInstance = CTraitInstance(shape);
//}

/*struct IMPORT_SWF_THREAD_PARAMETER
{
    gldMainMovie *pMainMovie;
    CSWFParse *pParse;
    gldLibrary *pTempLib;
	CDlgProgress *pDlg;
	LPCTSTR lpszFilePath;
	BOOL bSilenceMode;
};*/

/*
DWORD WINAPI ImportSWFThread(LPVOID lpParameter)
{
	UINT nIDResult = IDOK;

	IMPORT_SWF_THREAD_PARAMETER *pParam = (IMPORT_SWF_THREAD_PARAMETER *)lpParameter;

	CDlgProgress *pDlg = pParam->pDlg;
	pDlg->WaitForInitialize();

    //CFile _ff;
    //CFileException e;
    //if (!_ff.Open(pParam->lpszFilePath, CFile::modeRead | CFile::shareDenyWrite, &e))
    //{
    //    e.ReportError();
    //    nIDResult = IDCANCEL;
    //}
    //else
    {
        //U32 _fileSize = _ff.GetLength();
        //_ff.Close();

	    CSubject sub;
	    CSubjectManager::Instance()->Register("ImportSWF", &sub);

	    CSubjectManager::Instance()->GetSubject("ImportSWF")->Attach(pDlg);

        pDlg->SetCancelFlagPtr(pParam->pParse->GetCanceledFlagPtr());

        // parse the file
        if (!pParam->pParse->ParseFileStructure(pParam->lpszFilePath))
		{
			if (!pParam->bSilenceMode)
			{
				CString str;
				str.Format(IDS_ERROR_SWF_FORMAT1, pParam->lpszFilePath);
				pDlg->MessageBox(str, 0, MB_ICONWARNING | MB_OK); 
				nIDResult = IDCANCEL;
			}
		}
		else
		{
			if (pDlg->IsCanceled())
				nIDResult = IDCANCEL;

			if (nIDResult != IDCANCEL)
			{
				if (!pParam->bSilenceMode)
				{
					if (pParam->pParse->m_movieVersion > 7)
					{
						CString str;
						str.Format(IDS_PROMPT_HIGHER_SWF_VERSION1, pParam->pParse->m_movieVersion);
						if (pDlg->MessageBox(str, 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
							nIDResult = IDCANCEL;
					}

					if (pParam->pParse->m_protected)
					{
						CString str;
						str.LoadString(IDS_PROMPT_PROTECTED_SWF);
						if (pDlg->MessageBox(str, 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
							nIDResult = IDCANCEL;
					}
				}
			}

			if (nIDResult != IDCANCEL)
				if (pDlg->IsCanceled())
					nIDResult = IDCANCEL;

			if (nIDResult != IDCANCEL)
			{
				// Create the temp library to import the movie
				// Import the movie
				gldSWFImporter _importer(pParam->pParse, pParam->pTempLib);
	        
				if (!_importer.ImportSWFMovie(pParam->pParse->m_tagList, pParam->pMainMovie->GetCurrentScene()))
				{
					if (!pDlg->IsCanceled())
					{
						CString str;
						str.LoadString(IDS_ERROR_IMPORT);
						pDlg->MessageBox(str, 0, MB_ICONWARNING | MB_YESNO);
					}
					nIDResult = IDCANCEL;
				}
				else
				{
					// Update gldText object's bounds

					CSubject *pSub = CSubjectManager::Instance()->GetSubject("ImportSWF");

					ITextTool IClass;

					GOBJECT_LIST &_olist = (GOBJECT_LIST &)pParam->pTempLib->GetObjList();
					int size = (int)_olist.size();
					int i = 1;
					for (GOBJECT_LIST_IT it = _olist.begin(); it != _olist.end(); it++, i++)
					{
						gldObj *_obj = (*it);

						if (_obj->IsGObjInstanceOf(gobjText))
						{
							gldText2 *_gt2 = (gldText2*)_obj;

							_gt2->m_bounds = _gt2->m_importBounds;

							SETextToolEx TTool(&IClass);

							TTool.ConvertFromGldText(*_gt2);
							TTool.ConvertToGldText(*_gt2);
							CTransAdaptor::UpdateTShape(_gt2);

							pSub->Notify((void *)(int)((float)i / size * 20 + 80));
						}

						if (pParam->pDlg->IsCanceled())
						{
							nIDResult = IDCANCEL;
							break;
						}
					}
		
					if (nIDResult != IDCANCEL)
					{
						pSub->Notify((void *)100);
						pParam->pDlg->SetCancelFlagPtr(NULL);

						pParam->pMainMovie->m_width = pParam->pParse->m_movieWidth;
						pParam->pMainMovie->m_height = pParam->pParse->m_movieHeight;
						pParam->pMainMovie->m_frameRate = pParam->pParse->m_movieRate;
						// Get the r, g, b value from main movie color
						U8 _a = 0xff;
						U8 _r = U8((pParam->pParse->m_movieBKColor >> 16) & 0xff);
						U8 _g = U8((pParam->pParse->m_movieBKColor >> 8) & 0xff);
						U8 _b = U8(pParam->pParse->m_movieBKColor & 0xff);
						pParam->pMainMovie->m_color = (_a << 24) | (_b << 16 ) | (_g << 8) | _r;

						// Parse the motion tween of the main movie
						pParam->pMainMovie->GetCurrentScene()->ParseMotionTween();

						pSub->Notify((void *)100);
					}
				}
			}
        }

	    CSubjectManager::Instance()->UnRegister("ImportSWF");
    }

	pDlg->End(nIDResult);

	return 0;
}
*/

BOOL CGlandaDoc::ImportSWF(LPCTSTR lpszFile)
{
	if (PathFileExists(lpszFile))
	{
		CCategoryWnd::Instance()->SetActivePage(INDEX_FILES_PAGE);
		HTREEITEM hItem = CCategoryWnd::Instance()->m_CategoryImportPage.m_ExplorerWnd.m_pShellTreeCtrl->Locate(lpszFile);
		if (hItem)
		{
			CCategoryWnd::Instance()->m_CategoryImportPage.m_ExplorerWnd.m_pShellTreeCtrl->Expand(hItem, TVE_EXPAND);
			return TRUE;
		}
	}

	AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_FAILED_IMPORT_FILE_1, lpszFile);
	return FALSE;
}

BOOL CGlandaDoc::ImportGLC(LPCTSTR pszFile, CPoint point)
{
	LeaveEditControl();
	my_app.CurTool()->Leave();

	CWaitCursor wc;

	GOBJECT_LIST lstObj;
	CAutoPtr<gldInstance> inst(new gldInstance);	
	if (FAILED(inst->LoadFromFile(pszFile, lstObj)))
	{
		AfxMessageBox(IDS_FAILED_IMPORT_FILE, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	TCommandGroup *cmd = new TCommandGroup(IDS_CMD_IMPORTRESOURCE);	
	for (GOBJECT_LIST_IT i = lstObj.begin(); i != lstObj.end(); ++i)
	{	
		if ((*i)->m_name.length() == 0)
		{
			gldDataKeeper::Instance()->m_objLib->GenerateNextSymbolName((*i)->m_name);
		}
		//(*i).second->UpdateUniId(); // CCmdAddObj do this in his constructor
		cmd->Do(new CCmdAddObj(*i));
	}
	int	x = point.x * 20;
	int	y = point.y * 20;
	my_app.ScreenToDataSpace(x, y);	
	ASSERT(inst->m_obj != NULL);
	int tx = (inst->m_obj->m_bounds.left + inst->m_obj->m_bounds.right) / 2;
	int ty = (inst->m_obj->m_bounds.top + inst->m_obj->m_bounds.bottom) / 2;
	int cx = inst->m_matrix.GetX(tx, ty);
	int cy = inst->m_matrix.GetY(tx, ty);
	inst->m_matrix.m_x += x - cx;
	inst->m_matrix.m_y += y - cy;
	cmd->Do(new CCmdAddInstance(_GetCurScene2(), inst.Detach()));
	my_app.Commands().Do(cmd);

	return TRUE;
}

BOOL CGlandaDoc::ImportGLS(LPCTSTR lpszFile, CPoint point)
{
	return ImportGLC(lpszFile, point);
}

BOOL CGlandaDoc::ImportVectorFormat(LPCTSTR lpszFile, CPoint point)
{
	LeaveEditControl();
	my_app.CurTool()->Leave();

	CWaitCursor wc;

	CString tmp_file_name = GetTempFile(NULL, "~vector", "swf");

	int ret = -1;

	CString ext = ::PathFindExtension(lpszFile);

	// get type of file according to extension name
	if (ext.CompareNoCase(".wmf") == 0)
		ret = ConvertWMFtoSWF(lpszFile, tmp_file_name);
	else if (ext.CompareNoCase(".emf") == 0)
		ret = ConvertEMFtoSWF(lpszFile, tmp_file_name);
	else if (ext.CompareNoCase(".svg") == 0)
		ret = ConvertSVGtoSWF(lpszFile, tmp_file_name);
	else if (ext.CompareNoCase(".ai") == 0)
		ret = ConvertAItoSWF(lpszFile, tmp_file_name);

	if (ret < 0)
	{
		::DeleteFile(tmp_file_name);
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, lpszFile);
		return FALSE;
	}
	// import vector from swf file	
	gldLibrary *main_lib = gldDataKeeper::Instance()->m_objLib;	
	gldLibrary tmp_lib;
	CTransAdaptor::SetAdaptor(&tmp_lib);	
	gldDataKeeper::Instance()->m_objLib = &tmp_lib;	
	CSWFParse parse;
	parse.m_swfName = tmp_file_name;
	if (parse.ParseFileStructure(tmp_file_name, false))
	{
		gldMovieClip *new_mc = new gldMovieClip;
		gldSWFImporter importer(&parse, &tmp_lib);
		if (importer.ImportSWFMovie(parse.m_tagList, new_mc))
		{	
			// import vector image as a group
			new_mc->m_editFlags |= gldMovieClip::FLAG_GROUP;
			gldDataKeeper::Instance()->m_objLib = main_lib;			
			bool inst = my_app.CanAddShape();
			// select file name that exclude path as name of sprite
			LPCTSTR file_name = lpszFile + strlen(lpszFile) - 1;
			while (file_name >= lpszFile 
				&& *file_name != '\\' 
				&& *file_name != '/')
				file_name--;
			file_name++;	// skip slash '\'
			ITextTool IClass;
			const GOBJECT_LIST &_olist = tmp_lib.GetObjList();			
			for (GOBJECT_LIST::const_iterator it = _olist.begin(); it != _olist.end(); it++)
			{
				gldObj *_obj = (*it);
				if (_obj->IsGObjInstanceOf(gobjText))
				{
					gldText2 *_gt2 = (gldText2*)_obj;
					_gt2->m_bounds = _gt2->m_bounds;
					SETextToolEx TTool(&IClass);
					TTool.ConvertFromGldText(*_gt2);
					TTool.ConvertToGldText(*_gt2);
					CTransAdaptor::UpdateTShape(_gt2);
				}				
			}
			// insert objects and instance used command
			if (inst)
			{
				int	x = point.x * 20;
				int	y = point.y * 20;
				my_app.ScreenToDataSpace(x, y);
				my_app.Commands().Do(new CCmdImportVector(&tmp_lib, new_mc, file_name, x, y));
			}
			else
			{
				my_app.Commands().Do(new CCmdImportVector(&tmp_lib, new_mc, file_name, false));
			}
			// clear objects list of temporary library avoid objects haven been deleted
			tmp_lib.ClearList();
		}
		else
		{
			delete new_mc;
			ASSERT(FALSE);
			::DeleteFile(tmp_file_name);
			AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, lpszFile);
			return FALSE;
		}
	}
	else
	{
		ASSERT(FALSE);
		::DeleteFile(tmp_file_name);
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, lpszFile);
		return FALSE;
	}

	::DeleteFile(tmp_file_name);

	return TRUE;
}


gldShape* _CreateShapeWrapImageEx(gldImage *gimage, BOOL bClipped, BOOL bFitToMovieSize)
{
	TImage	*timage = CTraitImage(gimage);
	ASSERT(timage != NULL);
	if (timage == NULL)
	{
		return NULL;
	}
	gld_shape image;
	CAutoPtr<TBitmapFillStyle> fs;
	if (bFitToMovieSize && (!bClipped)) // this an tiled bitmap with movie size
	{
		fs.Attach(new TTiledBitmapFillStyle(timage));
	}
	else
	{
		fs.Attach(new TClippedBitmapFillStyle(timage));
	}
	int x, y, width, height;
	if (bFitToMovieSize)
	{
		width = PIXEL_TO_TWIPS(_GetMainMovie2()->m_width);
		height = PIXEL_TO_TWIPS(_GetMainMovie2()->m_height);
	}
	else
	{
		width = PIXEL_TO_TWIPS(timage->Width());
		height = PIXEL_TO_TWIPS(timage->Height());
	}
	x = -(width / 2 - 7); // fix flash player bug which pixel can't align
	y = -(height / 2 - 7);
	
	image = TShapeBuilder::BuildRect(x, y, x + width, y + height, NULL, fs);

	gldShape	*pgShape = new gldShape;
	gld_shape	*ptShape = new gld_shape(image);
	pgShape->m_ptr = ptShape;
	CTransAdaptor::TShape2GShape(*ptShape, *pgShape);

	return pgShape;	
}

BOOL CGlandaDoc::ImportImage(LPCTSTR lpszFile, CPoint point, BOOL bShowImportDialog)
{
	LeaveEditControl();
	my_app.CurTool()->Leave();

	CString strFile = lpszFile;

	BOOL bBackground = FALSE;
	BOOL bTiled = TRUE;

	if(bShowImportDialog)
	{
		CImportImageDialog dlg;
		if(dlg.m_bShow)
		{
			if (dlg.DoModal() != IDOK)
				return FALSE;

			bBackground= dlg.m_bBackground;
			bTiled = dlg.m_bTiled;
		}
	}

	CWaitCursor wc;

	gldImage *_gimg = new gldImage();
    if (!_gimg->ReadImageData(strFile))
	{
		delete _gimg;
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_ERROR_IMPORT1, lpszFile);
		return FALSE;
	}

	CSWFParse	parse;
	CTransAdaptor::CreateTShapePtr(&parse, _gimg);

	::PathStripPath(strFile.GetBuffer());
	strFile.ReleaseBuffer();
	_gimg->m_name = strFile;

	//_gimg->UpdateUniId(); // this is do in CCmdAddObj's constructor????	
	TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_ADDIMAGE);

	pCmdGroup->Do(new CCmdAddObj(_gimg));

	if (CDesignWnd::Instance()->CanDraw(false))
	{
		// create a shape fill with this image
		//gldShape *_gshape = CSWFProxy::CreateShapeWrapImage(_gimg);
		gldShape *_gshape = _CreateShapeWrapImageEx(_gimg, !bTiled, bBackground);
		if (_gshape)
		{
			// General symbol UNIID and name
			//_gshape->UpdateUniId();
			_GetObjectMap()->UpdateUniId(_gshape);
			_GetObjectMap()->GenerateNextSymbolName(_gshape->m_name);


			// add shape to library
			pCmdGroup->Do(new CCmdAddObj(_gshape));

			gldMatrix _matrix;
			if (!bBackground)
			{
				int	_x = point.x * 20;
				int	_y = point.y * 20;
				my_app.ScreenToDataSpace(_x, _y);

				_matrix.m_x = _x;
				_matrix.m_y = _y;
			}
			else
			{
				CSize size = my_app.GetMovieSize();
				_matrix.m_x = size.cx * 20 / 2;
				_matrix.m_y = size.cy * 20 / 2;
			}

			CGuardDrawOnce xDraw;

			pCmdGroup->Do(new CCmdAddInstance(_GetCurScene2(), _gshape, _matrix, gldCxform()));

			if (bBackground)
				pCmdGroup->Do(new CCmdSendToBack(my_app.CurSel(), 1));
		}
	}

	my_app.Commands().Do(pCmdGroup);

	return TRUE;
}

void CGlandaDoc::OnEditUngroup()
{
	if (my_app.CurSel().count() == 1 
		&& CCmdBreakApart::CanBreakApart(*my_app.CurSel().begin())
		&& !((gldObj *)CTraitInstance(*my_app.CurSel().begin()))->IsGObjInstanceOf(gobjText))
	{
		my_app.Commands().Do(new CCmdBreakApart(*my_app.CurSel().begin()));
	}
}

void CGlandaDoc::OnUpdateEditUngroup(CCmdUI *pCmdUI)
{
	
	pCmdUI->Enable(my_app.CurSel().count() == 1 
		&& CCmdBreakApart::CanBreakApart(*my_app.CurSel().begin())
		&& !((gldObj *)CTraitInstance(*my_app.CurSel().begin()))->IsGObjInstanceOf(gobjText));	
}

BOOL CGlandaDoc::SaveModified()
{
	// TODO: Add your specialized code here and/or call the base class

	LeaveEditControl();

	my_app.CurTool()->Leave();

	return CDocument::SaveModified();
}

BOOL CGlandaDoc::DoFileSave()
{
	DWORD dwAttrib = GetFileAttributes(m_strPathName);
	if (dwAttrib != 0xffffffff && dwAttrib & FILE_ATTRIBUTE_READONLY)
	{
		if (AfxMessageBoxEx(MB_ICONWARNING | MB_OKCANCEL, IDS_ERROR_READONLY_FILE1, (LPCTSTR)m_strPathName) == IDCANCEL)
			return FALSE;

		// we do not have read-write access or the file does not (now) exist
		if (!DoSave(NULL))
		{
			// CIPagenieDoc::OnSaveDocument() will give an error message if failed to save file
			TRACE0("Warning: File save with new name failed.\n");
			return FALSE;
		}
	}
	else
	{
		if (!DoSave(m_strPathName))
		{
			TRACE0("Warning: File save failed.\n");
			return FALSE;
		}
	}
	return TRUE;
}

// Grid
void CGlandaDoc::OnGridShowgrid()
{
	// TODO: Add your command handler code here
	my_app.ShowGrid(!my_app.IsShowGrid());
}

void CGlandaDoc::OnUpdateGridShowgrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(my_app.IsShowGrid());
}

void CGlandaDoc::OnGridSnaptogrid()
{
	// TODO: Add your command handler code here
	my_app.SnapToGrid(!my_app.IsSnapToGrid());
}

void CGlandaDoc::OnUpdateGridSnaptogrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(my_app.IsSnapToGrid());
}

void CGlandaDoc::OnGridEditgrid()
{
	// TODO: Add your command handler code here
	CPropGrid	dlg;

	dlg.DoModal();
}

CString CGlandaDoc::GetExportMovieName()
{
	CString strName;
	if (!m_strMovieExport.IsEmpty())
	{
		strName = m_strMovieExport;
		if (!strName.IsEmpty())
		{
			TCHAR *pBuf = strName.GetBuffer(0);
			::PathStripPath(pBuf);
			::PathRemoveExtension(pBuf);
			strName.ReleaseBuffer();
			strName +=".swf";
		}
	}
	else
	{
		strName = GetTitle();
		if (!strName.IsEmpty())
		{
			::PathRemoveExtension(strName.GetBuffer(0));
			strName.ReleaseBuffer();
			strName +=".swf";
		}
	}
	return strName;
}


/*
void CGlandaDoc::OnFileExportjsfl()
{
	// TODO: Add your command handler code here	
	//CJSFLBuilder	builder;

	//builder.Build("c:\\test\\jsfl\\test.jsfl");
	
if (!CheckJSFLAssociation(TRUE))
		return;

	char flaFileName[MAX_PATH + 1];

	strcpy(flaFileName, (LPCTSTR)GetPathName());
	strcpy(flaFileName, PathFindFileName(flaFileName));
	*PathFindExtension(flaFileName) = '\0';
	if (strlen(flaFileName) == 0)
		strcpy(flaFileName, (LPCTSTR)GetTitle());
	if (strlen(flaFileName) == 0)
		strcpy(flaFileName, "noname");

	CFileDialog	dlg(FALSE, "fla", flaFileName, OFN_OVERWRITEPROMPT,
		"Flash source file(*.fla)|*.fla");	

	if (dlg.DoModal() == IDOK)
	{
		AfxGetApp()->DoWaitCursor(1);

		CString jsflFile = GetTempFile(NULL, "~", "jsfl");

		CJSFLBuilder	builder;

		builder.Build(jsflFile);

		ShellExecute(NULL, "open", jsflFile, NULL, NULL, SW_SHOWMAXIMIZED);

		AfxGetApp()->DoWaitCursor(-1);
	}
	CFLAFile	fla;

	const GOBJECT_LIST				lstObj = gldDataKeeper::Instance()->m_objLib->GetObjList();
	GOBJECT_LIST::const_iterator	i;

	// set symbol id
	int								id = 1;
	gldDataKeeper::Instance()->m_objLib->ClearAllObjID();
	for (i = lstObj.begin(); i != lstObj.end(); ++i)
		(*i)->m_id = id++;

	// add scene
	oFLABinStream os;

	//gldShape *emptyShape = new gldShape;
	//CFLASymbolCreator::CreateGraphic(emptyShape, os);
	//delete emptyShape;
    CFLASymbolCreator::CreateMovieClip(CSWFProxy::GetMainMovie()->GetCurrentScene(), os);
    SymbolData *sceneData = new SymbolData;
    vector<U8> *_streamData = os.GetStream();
    sceneData->dataBlock.assign(_streamData->begin(), _streamData->end());
	fla.AddScene(sceneData, 0);

	// build library	
	for (i = lstObj.begin(); i != lstObj.end(); ++i)
	{
		gldObj *pObj = *i;

		oFLABinStream	os1;
        SymbolData	*block = new SymbolData;
		if (pObj->IsGObjInstanceOf(gobjShape))
		{
			CFLASymbolCreator::CreateGraphic((gldShape *)pObj, os1);
            _streamData = os1.GetStream();
            block->dataBlock.assign(_streamData->begin(), _streamData->end());
			fla.AddSymbol(block, pObj->m_id, 0, pObj->m_name);
		}
		else if (pObj->IsGObjInstanceOf(gobjImage))
		{
			gldImage *image = (gldImage *)pObj;
			CFLASymbolCreator::CreateImage(image, os1);
            _streamData = os1.GetStream();
            block->dataBlock.assign(_streamData->begin(), _streamData->end());
			fla.AddBits(block, pObj->m_id, image->m_gimgType == gimageBMP ? 1 : 0, pObj->m_name);
		}
		else if (pObj->IsGObjInstanceOf(gobjSprite))
		{
			CFLASymbolCreator::CreateSprite((gldSprite *)pObj, os1);
            _streamData = os1.GetStream();
            block->dataBlock.assign(_streamData->begin(), _streamData->end());
			fla.AddSymbol(block, pObj->m_id, 2, pObj->m_name);
		}
		else if (pObj->IsGObjInstanceOf(gobjButton))
		{
			CFLASymbolCreator::CreateButton((gldButton *)pObj, os1);
            _streamData = os1.GetStream();
            block->dataBlock.assign(_streamData->begin(), _streamData->end());
			fla.AddSymbol(block, pObj->m_id, 1, pObj->m_name);
		}
		//else
		//	ASSERT(FALSE);
	}    

	CFile f;
    if (f.Open("c:\\test\\hello.jpg", CFile::modeRead))
    {
        U32 _len = f.GetLength();
        U8 *_buf = new U8[_len];
        f.Read(_buf, _len);
        f.Close();
        SymbolData _mediaBits1;
		oFLABinStream os;
		os.WriteArrayData(_buf, _len);
		os << (U32)0 << (U32)3200 << (U32)0 << (U32)3200;
		_mediaBits1.dataBlock = *(os.GetStream());		
        fla.AddBits(_mediaBits1, 1);        
    }
	fla.WriteFLAFile("c:\\test\\test.fla");

	ShellExecute(NULL, "open", "c:\\test\\test.fla", NULL, NULL, SW_SHOWMAXIMIZED);
}
*/

/*
void CGlandaDoc::OnSwapSymbol()
{
	// TODO: Add your command handler code here

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		gld_shape shape = *sel.begin();
		gldCharacterKey *pKey = CTraitCharacter(shape);

		CDlgSwapSymbol dlg;
		dlg.m_keys.push_back(pKey);
		
		if (dlg.DoModal() == IDOK)
		{
			ASSERT(dlg.m_pObjNew != NULL);
			GCHARACTERKEY_LIST keys;
			keys.push_back(pKey);
			my_app.Commands().Do(new CCmdSwapSymbol(keys, dlg.m_pObjNew));
		}
	}
}

void CGlandaDoc::OnUpdateSwapSymbol(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	gld_shape_sel sel = my_app.CurSel();
	if (sel.count() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}

	pCmdUI->Enable(FALSE);
}
*/

void CGlandaDoc::OnFileNewFromTemplate()
{
	// TODO: Add your command handler code here
	
_retry:
	CDlgTemplates dlg;
	if (dlg.DoModal() != IDOK)
		return;

	CLSID clsid;
	memset(&clsid, 0, sizeof(clsid));
	if (IsEqualCLSID(clsid, dlg.m_clsid))
	{
		if (SaveModified() == FALSE)
			return;
			
		InitDocument(TRUE);
		return;
	}

	HRESULT hr;

	CComPtr<IGLD_Application> pIApplication;
	hr = pIApplication.CoCreateInstance(__uuidof(GLD_Application));
	if (SUCCEEDED(hr))
	{
		CComPtr<IGLD_Template> pITemplate;
		hr = pITemplate.CoCreateInstance(dlg.m_clsid);
		if (FAILED(hr))
		{
			AfxMessageBox(IDS_FAILED_INIT_TEMPLATE);
			return;
		}

		if (SaveModified() == FALSE)
			return;
			
		if (!InitDocument(FALSE))
			return;

		CI2CMap I2CMap;
		CI2CMap *pOldI2CMap = CI2C::m_pI2CMap;
		CI2C::m_pI2CMap = &I2CMap;
		
		my_app.SetRepaint(false);
		my_app.SetRedraw(false);
		CDesignWnd::Instance()->m_bEraseBkgnd = TRUE;
		HRESULT hr = pITemplate->Execute(pIApplication);
		CDesignWnd::Instance()->m_bEraseBkgnd = FALSE;
		my_app.SetRedraw(true);
		my_app.SetRepaint(true);

		I2CMap.CopyTo(_GetObjectMap());
		CI2C::m_pI2CMap = pOldI2CMap;
		if (SUCCEEDED(hr))
		{
			my_app.Commands().Clear();
			my_app.Repaint();
			SetModifiedFlag(TRUE);
		}
		else
		{
			CComQIPtr<ISupportErrorInfo> pISEI = pITemplate;
			if (pISEI)
			{
				if (pISEI->InterfaceSupportsErrorInfo(__uuidof(IGLD_Template)) == S_OK)
				{
					CComQIPtr<IErrorInfo> pIEI;
					if (::GetErrorInfo(0, &pIEI) == S_OK)
					{
						CComBSTR bstrDes;
						if (pIEI->GetDescription(&bstrDes) == S_OK)
						{
							CString str(bstrDes);
							AfxMessageBox(str);
						}
					}
				}
			}

			InitDocument(TRUE);

			goto _retry;
		}
	}
}

// sample create text
HRESULT CreateStaticText(LPCTSTR pszText, COLORREF clText, IGLD_Text **ppIText)
{
	HRESULT hr;

	// create text object
	CComPtr<IGLD_StaticText> pIText;
	hr = pIText.CoCreateInstance(__uuidof(GLD_StaticText));
	CHECKRESULT();
	
	// create paragraph
	CComPtr<IGLD_TextParagraph> pIPara;
	hr = pIPara.CoCreateInstance(__uuidof(GLD_TextParagraph));
	CHECKRESULT();
	CComPtr<IGLD_ParagraphFormat> pIFmt;
	hr = pIPara->get_Format(&pIFmt);
	CHECKRESULT();
	hr = pIFmt->put_LeftMargin(200);
	
	// create text block
	CComPtr<IGLD_TextBlock> pIBlock;
	hr = pIBlock.CoCreateInstance(__uuidof(GLD_TextBlock));
	CHECKRESULT();

	// set text block properties
	hr = pIBlock->put_Text(CComBSTR(pszText));
	CHECKRESULT();
	CComPtr<IGLD_Font> pIFont;
	hr = pIBlock->get_Font(&pIFont);
	CHECKRESULT();
	CComPtr<IGLD_Color> pIColor;
	hr = pIFont->get_Color(&pIColor);
	CHECKRESULT();
	hr = pIColor->SetData(GetRValue(clText), GetGValue(clText), GetBValue(clText), 255);
	CHECKRESULT();
	hr = pIFont->put_Size(600);
	CHECKRESULT();

	// append text block to paragraph
	CComPtr<IGLD_TextBlocks> pIBlocks;
	hr = pIPara->get_TextBlocks(&pIBlocks);
	CHECKRESULT();	
	hr = pIBlocks->Append(pIBlock);
	CHECKRESULT();
	
	// append paragraph to text
	CComPtr<IGLD_TextParagraphs> pIParas;
	hr = pIText->get_Paragraphs(&pIParas);
	CHECKRESULT();
	hr = pIParas->Append(pIPara);
	CHECKRESULT();	

	// return the text object
	return pIText->QueryInterface(ppIText);
}

HRESULT CreateDynamicText(LPCTSTR pszText, IGLD_Text **ppIText)
{
	IF_ARG_NULL_RETURN(ppIText);
	IF_ARG_NULL_RETURN(pszText);

	HRESULT hr;

	CComPtr<IGLD_DynamicText> pIDyna;
	hr = pIDyna.CoCreateInstance(__uuidof(GLD_DynamicText));
	CHECKRESULT();

	hr = pIDyna->put_Text(CComBSTR(pszText));
	CHECKRESULT();

	hr = pIDyna->put_RenderAsHTML(VARIANT_TRUE);
	CHECKRESULT();

	return pIDyna->QueryInterface(ppIText);
}

inline SETextToolEx *GET_TEXTTOOL()
{
	return (SETextToolEx *)my_app.Tools()[IDT_TEXTTOOLEX - IDT_FIRST];
}

HRESULT TestReadPngFile(const CString &FileName)
{
	HRESULT hr;

	CComPtr<IGLD_FileStream> stream;
	hr = stream.CoCreateInstance(__uuidof(GLD_FileStream));
	CHECKRESULT();

	hr = stream->Open(CComBSTR(FileName), CComBSTR("rb"));
	CHECKRESULT();

	CComPtr<IGLD_Image> image;
	hr = image.CoCreateInstance(__uuidof(GLD_Image));
	CHECKRESULT();

	hr = image->Load(stream);
	CHECKRESULT();

	GLD_ImageDataFormat fmt;
	hr = image->get_Format(&fmt);
	CHECKRESULT();

	long width, height;
	image->get_Width(&width);
	image->get_Height(&height);

	TRACE("Width = %d; Height = %d\n", width, height);

	return (fmt == gcePngFormat) ? S_OK : E_FAIL;
}

HRESULT TestGLDParameters(void)
{
	HRESULT hr;

	CComPtr<IGLD_Parameters> pIParas;
	hr = pIParas.CoCreateInstance(__uuidof(GLD_Parameters));
	CHECKRESULT();
	hr = pIParas->put_Item(CComBSTR("Speed"), CComVariant(10));
	CHECKRESULT();
	hr = pIParas->put_Item(CComBSTR("Width"), CComVariant(300));
	CHECKRESULT();
	hr = pIParas->put_Item(CComBSTR("Height"), CComVariant(400));
	CHECKRESULT();
	hr = pIParas->put_Item(CComBSTR("Description"), CComVariant(CComBSTR("Test parameters")));
	CHECKRESULT();

	CComVariant var;
	hr = pIParas->get_Item(CComBSTR("Speed"), &var);
	CHECKRESULT();	
	VERIFY(var.vt == VT_I4 && var.iVal == 10);
	var.Clear();
	hr = pIParas->get_Item(CComBSTR("Width"), &var);
	CHECKRESULT();
	VERIFY(var.vt == VT_I4 && var.iVal == 300);
	var.Clear();
	hr = pIParas->get_Item(CComBSTR("Height"), &var);
	CHECKRESULT();
	VERIFY(var.vt == VT_I4 && var.iVal == 400);
	var.Clear();
	hr = pIParas->get_Item(CComBSTR("Description"), &var);
	CHECKRESULT();
	VERIFY(var.vt == VT_BSTR && CComBSTR(var.bstrVal) == CComBSTR("Test parameters")) ;
	var.Clear();

	CComPtr<IEnumVARIANT> enumVar;
	hr = pIParas->get__NewEnum(&enumVar);
	CHECKRESULT();
	USES_CONVERSION;
	while (enumVar->Next(1, &var, NULL) == S_OK)
	{
		CComQIPtr<IGLD_Parameter> para = var.punkVal;		
		if (para == NULL)
			hr = E_UNEXPECTED;
		CHECKRESULT();
		var.Clear();
		CComBSTR bstrName;
		hr = para->get_Name(&bstrName);
		CHECKRESULT();
		TRACE("Test Parameters: %s\n", OLE2A(bstrName));
	}

	return S_OK;
}

HRESULT TestFrameOperate()
{
	HRESULT hr;

	CComPtr<IGLD_Sprite> pISprite;
	hr = pISprite.CoCreateInstance(__uuidof(GLD_Sprite));
	CHECKRESULT();

	CComPtr<IGLD_MovieClip> pIMovie;
	hr = pISprite->get_MovieClip(&pIMovie);
	CHECKRESULT();

	CComPtr<IGLD_Layers> pILayers;
	hr = pIMovie->get_Layers(&pILayers);
	CHECKRESULT();

	CComPtr<IGLD_Layer> pILayer;
	hr = pILayers->Append(gceNormalLayer, NULL, &pILayer);
	CHECKRESULT();

	CComPtr<IGLD_FrameClips> pIFrameClips;
	hr = pILayer->get_FrameClips(&pIFrameClips);
	CHECKRESULT();

	pIFrameClips->Append(6, gceStaticFrameClip, NULL);
	pIFrameClips->Append(7, gceStaticFrameClip, NULL);
	pIFrameClips->Append(8, gceStaticFrameClip, NULL);

	pIFrameClips->Remove(8, 100);

	CComPtr<IEnumVARIANT> enumVar;
	pIFrameClips->get__NewEnum(&enumVar);
	CComVariant varFrame;
	TRACE("=====================Dump Frame Clips============================\n");
	while (enumVar->Next(1, &varFrame, NULL) == S_OK)
	{
		CComQIPtr<IGLD_FrameClip> pIFrame = varFrame.punkVal;
		varFrame.Clear();
		long startTime, length;
		pIFrame->get_StartTime(&startTime);
		pIFrame->get_Length(&length);

		TRACE("[%d, %d]", startTime, length);
	}
	TRACE("\n");

	return S_OK;
}


/*
void CGlandaDoc::OnTestCom()
{
	TestFrameOperate();

	return;

	if (FAILED(TestGLDParameters()))
		AfxMessageBox("Failed");

	if (FAILED(TestReadPngFile("c:\\test\\123.png")))
		AfxMessageBox("Failed");

	gldScene2 *pScene = _GetMainMovie2()->GetCurScene();
	ASSERT(pScene != NULL);

	HRESULT hr;
	CComPtr<IGLD_Text> pIText;
	hr = CreateStaticText("Hello, Glanda, aaaaaaaaaaaaaaaaaaaa", RGB(255, 0, 0), &pIText);
	if (FAILED(hr))
		return;
	gldText2 *pText;
	hr = CI2C::Create(pIText, &pText);
	if (FAILED(hr))
		return;

	int nMaxHeight = 0, nMaxWidth = -1;

	Paragraphs::iterator it_para = pText->m_ParagraphList.begin();
	while(it_para != pText->m_ParagraphList.end())
	{
		nMaxHeight += (*it_para)->GetHeight();
		it_para++;
	}

	it_para = pText->m_ParagraphList.begin();
	while(it_para != pText->m_ParagraphList.end())
	{
		nMaxWidth = max(nMaxWidth, (*it_para)->GetWidth() + (*it_para)->GetMarginLeft() + (*it_para)->GetMarginRight() + (*it_para)->GetIndent());
		it_para++;
	}
	pText->rtRegion.right = pText->rtRegion.left + nMaxWidth + BORDER_SIZE * 2;
	pText->rtRegion.bottom = pText->rtRegion.top + nMaxHeight + BORDER_SIZE * 2;

	//pText->UpdateUniId();
	_GetObjectMap()->UpdateUniId(pText);
	gldDataKeeper::Instance()->m_objLib->AddObj(pText);

	gldInstance *pInst = new gldInstance(pText, gldMatrix(), gldCxform());	
	pScene->m_instanceList.push_back(pInst);

	CTransAdaptor::RebuildCurrentScene();
	my_app.Redraw();
	my_app.Repaint();
}
*/

void CGlandaDoc::OnEditMovieProperties()
{
	CString strCaption;
	strCaption.LoadString(IDS_MOVIE_PROPERTIES);
	CMoviePropertiesSheet dlg(strCaption);

	dlg.DoModal();
}


void CGlandaDoc::OnExportResource()
{
	gld_shape_sel &sel = my_app.CurSel();
	if (sel.count() != 1)
		return;

	UINT nTypeName;
	TCHAR *pszFileExt = NULL;

	CWaitCursor wc;

	gldInstance *pInst = _GetCurInstance();
	ASSERT(pInst != NULL);
	ASSERT(pInst->m_obj != NULL);

	if (pInst->m_obj->IsGObjInstanceOf(gobjShape))
	{
		nTypeName = IDS_SYMBOL_SHAPE;
		pszFileExt = "gls";
	}
	else if (pInst->m_obj->IsGObjInstanceOf(gobjButton))
	{
		nTypeName = IDS_SYMBOL_BUTTON;
		pszFileExt = "glb";
	}
	else if (pInst->m_obj->IsGObjInstanceOf(gobjSprite))
	{
		nTypeName = IDS_SYMBOL_MOVIE_CLIP;
		pszFileExt = "glm";
	}
	else
	{
		AfxMessageBox(IDS_CANT_SAVE_ELEMENT);
		return;
	}

	CString strTypeName;
	strTypeName.LoadString(nTypeName);

	CString strFilter;
	strFilter.Format("%s (*.%s)|*.%s||", (LPCTSTR)strTypeName, pszFileExt, pszFileExt);

	CFileDialogEx dlg("Save Instance", FALSE, pszFileExt, strFilter, pInst->m_name.c_str());
	if (dlg.DoModal() == IDOK)
	{		
		if (FAILED(pInst->SaveToFile(dlg.GetPathName())))
		{			
			AfxMessageBox(IDS_FAILED_EXPORT_RESOURCE);
		}
	}
}

void CGlandaDoc::OnUpdateExportResource(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(my_app.CurSel().count() == 1);
}

void CGlandaDoc::OnEditCopy()
{
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Copy();
	}
	else
	{	
		Clipboard.Copy();
	}
}

void CGlandaDoc::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		CPoint ptStart, ptEnd;
		pTool->GetSel(ptStart, ptEnd);
		pCmdUI->Enable(ptStart != ptEnd);
	}
	else
	{
		pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForCopy));
	}	
}

void CGlandaDoc::OnEditCut()
{
	if (Clipboard.IsDataAvailable(CGlandaClipboard::ForCut))
		Clipboard.Cut();
}

void CGlandaDoc::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForCut));
}

void CGlandaDoc::OnEditPaste()
{
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Paste();
	}
	else
	{
		Clipboard.Paste();
	}
}

void CGlandaDoc::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(pTool->CanPaste());
	}
	else
	{
		pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForPaste));
	}
}

void CGlandaDoc::OnInsertSound()
{
	CEditMCReplaceDialog dlg(RESOURCE_SOUNDS);
	if (dlg.DoModal() == IDOK && dlg.GetCurSelResource())
	{
		CWaitCursor wc;

		CResourceThumbItem* pThumbItem = dlg.GetCurSelResource();
		gldSound* pSound = NULL;
		if(pThumbItem->m_bStockResource)
		{
			if(pThumbItem->m_pObj->IsGObjInstanceOf(gobjSound))
			{
				pSound = (gldSound*)(pThumbItem->m_pObj);
			}
			else
			{
				// error
				return;
			}
		}
		else
		{
			pSound = new gldSound();
			if (!pSound->ReadFile(pThumbItem->m_strPathName))
			{
				delete pSound;
				AfxMessageBox(IDS_ERROR_IMPORT);
				return;
			}
			pSound->m_name = FileStripPath(pThumbItem->m_strPathName);
			//pSound->UpdateUniId();
			_GetObjectMap()->UpdateUniId(pSound);
		}

		gldSceneSound *pSceneSound = new gldSceneSound();
		pSceneSound->m_sound->m_soundObj = pSound;
		pSceneSound->m_startTime = 0;
		pSceneSound->m_length = (int)max(1, pSound->GetSoundTime() * _GetMainMovie2()->m_frameRate);
		pSceneSound->m_name = pSound->m_name;

		gldScene2 *pScene = _GetCurScene2();
		if (pScene->m_soundList.size() > 0)
		{
			gldSceneSound *pCurSceneSound = *pScene->m_soundList.rbegin();
			pSceneSound->m_startTime = pCurSceneSound->m_startTime + pCurSceneSound->m_length;
		}

		TCommandGroup *pCmdGroup = new TCommandGroup(IDS_CMD_IMPORTSOUND);
		pCmdGroup->Do(new CCmdAddObj(pSound));
		pCmdGroup->Do(new CCmdAddSound(pSceneSound));
		my_app.Commands().Do(pCmdGroup);
	}
}

void CGlandaDoc::OnUpdateInsertSound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

void CGlandaDoc::OnEditDelete()
{
	// TODO: Add your command handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pTool->Key_Del();
	}
	else
	{
		Clipboard.Delete();
	}
}

void CGlandaDoc::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	SETextToolEx *pTool = GET_TEXTTOOL();
	if (pTool->IsActive())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(Clipboard.IsDataAvailable(CGlandaClipboard::ForDelete));
	}
}

void CGlandaDoc::OnEditSceneProperties()
{
	if (_GetCurScene2() != NULL)
	{
		CString strCaption;
		strCaption.LoadString(IDS_SCENE_PROPERTIES);
		CScenePropertiesSheet dlg(strCaption, NULL, 0);
		dlg.DoModal();
	}
}

void CGlandaDoc::OnUpdateEditSceneProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

void CGlandaDoc::OnInsertImage()
{
	CString strTitle, strFilter;
	strTitle.LoadString(IDS_IMPORT_IMAGE_TITLE);
	strFilter.LoadString(IDS_FILTER_IMAGE_ALL);
	CFileDialogEx dlg(strTitle, TRUE, NULL, strFilter);
	dlg.m_pOFN->lpstrTitle = strTitle;
	if (dlg.DoModal() == IDOK)
	{
		CString strFile = dlg.GetPathName();

		CSize size = my_app.GetViewSize();
		CPoint point(size.cx / 2, size.cy / 2);

		ImportImage(strFile, point, TRUE);
	}
}

void CGlandaDoc::OnUpdateInsertImage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}

void CGlandaDoc::OnInsertText()
{
	if (_GetCurScene2() != NULL)
	{
		CDlgInsertText dlg;
		INT_PTR ret = dlg.DoModal();
		if (ret == IDOK)
		{
		}
		else if (ret == ID_USE_TEXT_TOOL)
		{
			my_app.SetCurTool(IDT_TEXTTOOLEX);
		}
	}
}

void CGlandaDoc::OnUpdateInsertText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(_GetCurScene2() != NULL);
}