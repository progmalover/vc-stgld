// TestImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Glanda.h"
#include "TestImportDlg.h"

#include "Global.h"
#include "DlgDirectoryPicker.h"

#include "GlandaDoc.h"
#include "gldMovieClip.h"
#include "gldLibrary.h"
#include "gldObj.h"

#include "my_app.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDlgTestImport dialog

IMPLEMENT_SINGLETON(CDlgTestImport)
CDlgTestImport::CDlgTestImport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestImport::IDD, pParent)
	, m_bTestChangeFrame(FALSE)
	, m_nLoop(1)
{
	m_bRunning = FALSE;
	m_bStop = FALSE;
	
	m_strPath = AfxGetApp()->GetProfileString("Test Import", "SWF Path", NULL);
	m_bIncludeSubFolders = AfxGetApp()->GetProfileInt("Test Import", "Include Sub Folders", TRUE);

	m_bTestChangeFrame = AfxGetApp()->GetProfileInt("Test Import", "Test Change Frame", TRUE);
	m_bTestInplaceEdit = AfxGetApp()->GetProfileInt("Test Import", "Test Inplace Edit", FALSE);
}

CDlgTestImport::~CDlgTestImport()
{
	AfxGetApp()->WriteProfileString("Test Import", "SWF Path", m_strPath);
	AfxGetApp()->WriteProfileInt("Test Import", "Include Sub Folders", m_bIncludeSubFolders);

	AfxGetApp()->WriteProfileInt("Test Import", "Test Change Frame", m_bTestChangeFrame);
	AfxGetApp()->WriteProfileInt("Test Import", "Test Inplace Edit", m_bTestInplaceEdit);
}

void CDlgTestImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_list);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Check(pDX, IDC_CHECK_INCLUDE_SUB_FOLDERS, m_bIncludeSubFolders);
	DDX_Check(pDX, IDC_CHECK_TEST_CHANGE_FRAME, m_bTestChangeFrame);
	DDX_Check(pDX, IDC_CHECK_TEST_INPLACE_EDIT, m_bTestInplaceEdit);
	DDX_Text(pDX, IDC_EDIT_LOOP, m_nLoop);
	DDV_MinMaxInt(pDX, m_nLoop, 1, SHRT_MAX);
}

BEGIN_MESSAGE_MAP(CDlgTestImport, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_SWF_PATH, OnBnClickedButtonBrowseSwfPath)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, OnBnClickedButtonSelectall)
	ON_BN_CLICKED(IDC_BUTTON_DESELECTALL, OnBnClickedButtonDeselectall)
END_MESSAGE_MAP()


// CDlgTestImport message handlers

BOOL CDlgTestImport::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_LOOP))->SetRange(1, SHRT_MAX);

	if (!m_strPath.IsEmpty())
		OnBnClickedButtonRefresh();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlgTestImport::AddFiles(LPCTSTR lpszPath)
{
	CString strPath = lpszPath;
	strPath += "\\*.*";

	CFileFind ff;

	BOOL ret = ff.FindFile(strPath);
	while (ret)
	{
		ret = ff.FindNextFile();

		CString strFile = ff.GetFilePath();
		if (ff.IsDirectory())
		{
			if (m_bIncludeSubFolders & !ff.IsDots())
				AddFiles(strFile);
		}
		else
		{
			if (strFile.GetLength() >= 4 && strFile.Right(4).CompareNoCase(".swf") == 0)
			{
				int index = m_list.AddString(strFile);
				m_list.SetCheck(index, BST_CHECKED);
			}
		}
	}
}

void CDlgTestImport::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	m_list.ResetContent();

	CString strPath = m_strPath;
	strPath.TrimRight('\\');

	AddFiles(strPath);

	if (m_list.GetCount() > 0)
		m_list.SetCurSel(0);
}

void CDlgTestImport::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here

	if (!UpdateData(TRUE))
		return;

	if (m_list.GetCount() == 0)
	{
		MessageBeep(0);
		return;
	}

	if (m_bRunning)
	{
		m_bStop = TRUE;
		return;
	}

	m_bRunning = TRUE;
	m_bStop = FALSE;

	GetDlgItem(IDC_CHECK_TEST_CHANGE_FRAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_TEST_INPLACE_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_LOOP)->EnableWindow(FALSE);
	SetDlgItemText(IDC_BUTTON_START, "&Stop");
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_DISABLED | MF_GRAYED);

	for (int i = 0; i < m_nLoop && ! m_bStop; i++)
		RunTest();

	if (!m_bStop)
	{
		CString str;
		str.Format("Done.\r\n\r\n   Tested: %d\r\n   Import failure: %d\r\n   Export failure: %d\r\n\r\nThe details were saved to c:\\testimport.log.", m_nChecked, m_nImportFailed, m_nExportFailed);
		MessageBox(str, 0, MB_ICONINFORMATION | MB_OK);
	}

	GetDlgItem(IDC_CHECK_TEST_CHANGE_FRAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_TEST_INPLACE_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LOOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_LOOP)->EnableWindow(TRUE);
	SetDlgItemText(IDC_BUTTON_START, "&Start");
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_ENABLED);

	m_bRunning = FALSE;
}

void AppendLog(CFile *pFile, LPCTSTR fmt, ...)
{
	CString strBuf;

	va_list args;
	va_start(args, fmt);
	strBuf.FormatV(fmt, args);
	va_end (args);

	pFile->SeekToEnd();
	pFile->Write(strBuf, strBuf.GetLength());
}

void DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!::GetMessage(&msg, NULL, 0, 0))
			AfxPostQuitMessage(0);

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void CDlgTestImport::RunTest()
{
	return;

	/*
	UINT nFlags = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite;
	//if (m_list.GetCurSel() > 0)
	//	nFlags |= CFile::modeNoTruncate;

	CFile *pFile = NULL;
	try
	{
		pFile = new CFile ("c:\\testimport.log", nFlags);
	}
	catch (CFileException *e)
	{
		e->ReportError();
		e->Delete();
		delete pFile;
		return;
	}

	CGlandaDoc *pDoc = _GetCurDocument();
	pDoc->m_bSilentMode = TRUE;

	int nCount = m_list.GetCount();
	m_nChecked = 0;
	m_nImportFailed = 0;
	m_nExportFailed = 0;
	for (int i = 0; i < nCount; i++)
	{
		if (m_list.GetCheck(i) == BST_CHECKED)
			m_nChecked++;
	}

	int nTested = 0;
	for (int i = 0; i < m_list.GetCount(); i++)
	{
		if (m_list.GetCheck(i) != BST_CHECKED)
			continue;

		nTested++;
		CString strText;
		strText.Format("Debug: Test Import (%d / %d / %d)", nTested, m_nChecked, nCount);
		SetWindowText(strText);


		m_list.SetSel(-1, 0);
		m_list.SetSel(i, 1);
		m_list.SetTopIndex(i);
		m_list.UpdateWindow();

		CString strSWF;
		m_list.GetText(i, strSWF);


		// import
		AppendLog(pFile, "%s: Import %s.", CTime::GetCurrentTime().Format("%H:%M:%S"), strSWF);

		// do import
		BOOL ret = pDoc->ImportSWF(strSWF, FALSE);

		// done
		AppendLog(pFile, " [%s]\r\n", ret ? "Done" : "Failed");

		if (!ret)
		{
			m_nImportFailed++;
		}
		else
		{
			my_app.ZoomTo(10);

			// change frame
			if (m_bTestChangeFrame)
			{
				AppendLog(pFile, "%s: Change Frame:", CTime::GetCurrentTime().Format("%H:%M:%S"));

				gldMovieClip *pMC = _GetCurMovieClip();
				U16 nMaxTime = pMC->GetMaxTime();
				for (int j = 0; j < nMaxTime; j++)
				{
					DoEvents();
					if (m_bStop)
						break;

					AppendLog(pFile, " %d", j);
					//pDoc->ChangeCurrentTime(j);
				}

				AppendLog(pFile, " [Done]\r\n");
			}

			if (m_bTestInplaceEdit)
			{
				GOBJECT_LIST _objList = gldDataKeeper::Instance()->m_objLib->GetObjList();
				for (GOBJECT_LIST_IT it = _objList.begin(); it != _objList.end(); it++)
				{
					DoEvents();
					if (m_bStop)
						break;

					gldObj *pObj = *it;
					if (pObj->IsGObjInstanceOf(gobjSprite))
					{
						//CLibraryTree *pTree = CLibraryTree::Instance();
						//pTree->UnselectAllItems();
				
						//HTREEITEM hItem = pTree->FindObj(TVI_ROOT, pObj);
						//pTree->EnsureVisible(hItem);
						//pTree->SelectItem(hItem);

						AppendLog(pFile, "%s: Change MovieClip: %s.", CTime::GetCurrentTime().Format("%H:%M:%S"), pObj->m_name.c_str());
						//_GetCurDocument()->ChangeCurrentMovieClip(pObj);
						pTree->EditSymbol(hItem);
						AppendLog(pFile, " [Done]\r\n");

						// change frame
						if (m_bTestChangeFrame)
						{
							AppendLog(pFile, "%s: Change Frame:", CTime::GetCurrentTime().Format("%H:%M:%S"));

							gldMovieClip *pMC = _GetCurMovieClip();
							U16 nMaxTime = pMC->GetMaxTime();
							for (int j = 0; j < nMaxTime; j++)
							{
								DoEvents();
								if (m_bStop)
									break;

								AppendLog(pFile, " %d", j);
								//pDoc->ChangeCurrentTime(j);
							}

							AppendLog(pFile, " [Done]\r\n");
						}
					}
				}			
			}
		}

		DoEvents();
		if (m_bStop)
			break;

		// export
		CString strFile = GetTempFile(NULL, "~preview", "swf");

		AppendLog(pFile, "%s: Export %s.", CTime::GetCurrentTime().Format("%H:%M:%S"), strFile);

		// do export
		ret = ExportMovie(AfxGetMainWnd(), strFile, FALSE, FALSE);
		if (!ret)
			m_nExportFailed++;

		// done
		AppendLog(pFile, " [%s]\r\n", ret ? "Done" : "Failed");
		::DeleteFile(strFile);

		DoEvents();
		if (m_bStop)
			break;
	}

	pDoc->m_bSilentMode = FALSE;

	delete pFile;
	*/
}

void CDlgTestImport::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	m_bStop = TRUE;

	CDialog::OnClose();
}

void CDlgTestImport::OnBnClickedButtonBrowseSwfPath()
{
	// TODO: Add your control notification handler code here

	CDlgDirectoryPicker dlg(this);
	dlg.SetPath(m_strPath);
	if (dlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_PATH, dlg.GetPath());
		OnBnClickedButtonRefresh();
	}
}

void CDlgTestImport::OnBnClickedButtonSelectall()
{
	// TODO: Add your control notification handler code here

	for (int i = 0 ; i < m_list.GetCount(); i++)
		m_list.SetCheck(i, BST_CHECKED);
}

void CDlgTestImport::OnBnClickedButtonDeselectall()
{
	// TODO: Add your control notification handler code here

	for (int i = 0 ; i < m_list.GetCount(); i++)
		m_list.SetCheck(i, BST_UNCHECKED);
}
