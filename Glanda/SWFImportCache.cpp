#include "StdAfx.h"
#include "SWFImportCache.h"
#include "Crc32Static.h"
#include "SWFParse.h"
#include "gldLibrary.h"
#include "gldSWFImporter.h"
#include "gldDataKeeper.h"
#include "gldMovieClip.h"
#include "gldObj.h"
#include "gldImage.h"
#include "gldSound.h"
#include "Glanda_i.h"
#include "C2I.h"
#include "TransAdaptor.h"
#include "TextToolEx.h"
#include "ITextTool.h"
#include "toolsdef.h"
#include "BinStream.h"
#include "filepath.h"
#include <set>
using namespace std;

#define MAX_CACHE_TABLE 32768
#define WAIT_FILE_ACCESS_TIME	500

// helper function
inline bool IsEqualFileTime(const FILETIME &ft1, const FILETIME &ft2)
{
	return ft1.dwLowDateTime == ft2.dwLowDateTime && ft1.dwHighDateTime == ft2.dwHighDateTime;
}

BOOL RemoveDirectoryRecursively(const CString &path)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(path + "\\*.*", &wfd);
	if (hFind != NULL)
	{
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(wfd.cFileName, ".") != 0 &&
					_tcscmp(wfd.cFileName, "..") != 0)
				{
					CString subPath;
					subPath.Format("%s\\%s", (LPCTSTR)path, wfd.cFileName);
					
					if (!RemoveDirectoryRecursively(subPath))
						return FALSE;
				}
			}
			else
			{
				CString sFileName;
				sFileName.Format("%s\\%s", (LPCTSTR)path, wfd.cFileName);
				DeleteFile(sFileName);
			}
		} while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
		
		if (!RemoveDirectory(path))
		{
		#ifdef _DEBUG
			CString str = ::GetLastErrorMessage();
			AfxMessageBoxEx(0,"Failed to delete %s: %s.", (LPCTSTR)path, (LPCTSTR)str);
		#endif
			return FALSE;
		}
	}
	return !PathFileExists(path);
}

struct _ImportInfo
{
	GObjType type;
	int num;
	CString path;
	CString folder;
};

BOOL ExportSSObject(gldObj *pObj, const CString &sPath)
{
	TCHAR szFileExt[10];
	
	if (pObj->IsGObjInstanceOf(gobjShape))
		_tcscpy(szFileExt, "gls");
	else if (pObj->IsGObjInstanceOf(gobjButton))
		_tcscpy(szFileExt, "glb");
	else if (pObj->IsGObjInstanceOf(gobjSprite))
		_tcscpy(szFileExt, "glm");
	else
		_tcscpy(szFileExt, "glc");		

	CString sFileName;
	sFileName.Format("%s\\%s.%s", (LPCTSTR)sPath, pObj->m_name.c_str(), szFileExt);

	CComPtr<IGLD_Object> pIObject;

	HRESULT hr;

	CC2IMap C2IMap;
	CC2IMap *pOldMap = CC2I::m_pC2IMap;
	CC2I::m_pC2IMap = &C2IMap;
	hr = CC2I::Create(pObj, &pIObject);
	CC2I::m_pC2IMap = pOldMap;
	if (FAILED(hr))
		return FALSE;

	CComPtr<IStorage> pStg;
	USES_CONVERSION;
	hr = StgCreateDocfile(A2OLE(sFileName), STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStg);
	if (FAILED(hr))
		return FALSE;

	CComQIPtr<IPersistStorage> pPStg = pIObject;
	if (pPStg == NULL)
		hr = E_NOINTERFACE;
	if (FAILED(hr))
		return FALSE;

	hr = OleSave(pPStg, pStg, FALSE);
	if (FAILED(hr))
		return FALSE;
	
	return TRUE;
}

BOOL ExportImage(gldImage *pImage, const CString &sPath)
{
	ASSERT(pImage != NULL);
	
	GImageType type = pImage->m_gimgType;
	const char *ext = "unk";
	if (type == gimageJPEG)
		ext = "jpg";
	else if (type == gimagePNG)
		ext = "png";
	else if (type == gimageBMP)
		ext = "bmp";
	else	
		ASSERT(FALSE);
	
	ASSERT(pImage->m_name.size() > 0);

	CString sFilePath;
	sFilePath.Format("%s\\%s.%s", (LPCTSTR)sPath, pImage->m_name.c_str(), ext);

	CStdioFile file;
	if (!file.Open(sFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		return FALSE;

	const U32 maxUINT = (U32)(~(UINT)0);
	U32 remainLen = pImage->GetRawDataLen();
	U8 *pData = pImage->GetRawData();
	try
	{
		while (remainLen > 0)
		{
			UINT writeLen = (remainLen > maxUINT ? maxUINT : remainLen);
			file.Write(pData, writeLen);
			remainLen -= writeLen;
			pData += writeLen;
		}
	}
	catch (CFileException *e)
	{
		e->Delete();
		file.Close();
		DeleteFile(sPath);
		return FALSE;
	}

	return TRUE;
}

BOOL ExportSound(gldSound *pSound, const CString &sPath)
{
	ASSERT(pSound != NULL);
	
	U8 fmt = pSound->GetSoundFormat();
	const char *ext = "unk";
	if (fmt == SOUND_FORMAT_MP3)
		ext = "mp3";
	else if (fmt == SOUND_FORMAT_WAV)
		ext = "wav";	
	else	
		ASSERT(FALSE);
	
	ASSERT(pSound->m_name.size() > 0);

	CString sFilePath;
	sFilePath.Format("%s\\%s.%s", (LPCTSTR)sPath, pSound->m_name.c_str(), ext);

	CStdioFile file;
	if (!file.Open(sFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		return FALSE;

	const U32 maxUINT = (U32)(~(UINT)0);
	U32 remainLen = pSound->GetRawDataLen();
	U8 *pData = pSound->GetRawData();
	try
	{
		while (remainLen > 0)
		{
			UINT writeLen = (remainLen > maxUINT ? maxUINT : remainLen);
			file.Write(pData, writeLen);
			remainLen -= writeLen;
			pData += writeLen;
		}
	}
	catch (CFileException *e)
	{
		e->Delete();
		file.Close();
		DeleteFile(sPath);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// helper struct
//////////////////////////////////////////////////////////////////////////////////////
typedef CSWFImportCache::_MyCache::iterator _MyCacheIt;
struct _SystemTimeLess
{
	bool operator()(const _MyCacheIt &i1, const _MyCacheIt &i2) const
	{
		const SYSTEMTIME &ft1 = (*i1).second.lastAccessTime;
		const SYSTEMTIME &ft2 = (*i2).second.lastAccessTime;

		if (ft1.wYear < ft2.wYear)
			return TRUE;
		else if (ft1.wYear > ft2.wYear)
			return FALSE;

		if (ft1.wMonth < ft2.wMonth)
			return TRUE;
		else if (ft1.wMonth > ft2.wMonth)
			return FALSE;

		if (ft1.wDay < ft2.wDay)
			return TRUE;
		else if (ft1.wDay > ft2.wDay)
			return FALSE;

		if (ft1.wHour < ft2.wHour)
			return TRUE;
		else if (ft1.wHour > ft2.wHour)
			return FALSE;

		if (ft1.wMinute < ft2.wMinute)
			return TRUE;
		else if (ft1.wMinute > ft2.wMinute)
			return FALSE;

		if (ft1.wSecond < ft2.wSecond)
			return TRUE;
		else if (ft1.wSecond > ft2.wSecond)
			return FALSE;

		if (ft1.wMilliseconds < ft2.wMilliseconds)
			return TRUE;
		else if (ft1.wMilliseconds > ft2.wMilliseconds)
			return FALSE;

		return FALSE;
	}
};
typedef multiset<_MyCacheIt, _SystemTimeLess> _FTSortQueue;

class CCoInit
{
public:
	CCoInit()
	{
		m_Result = CoInitialize(NULL);
	}

	~CCoInit()
	{
		if (SUCCEEDED(m_Result))
			CoUninitialize();
	}

	HRESULT m_Result;
};

////////////////////////////////////////////////////////////////////////////////////////

CSWFImportCache::CSWFImportCache(bool bCache /*= true*/)
: m_pfCallback(NULL)
, m_lParam(0)
, m_pbCanceled(NULL)
, m_hThread(NULL)
, m_bResult(TRUE)
, m_bCache(bCache)
{
}

CSWFImportCache::~CSWFImportCache()
{
}

CString CSWFImportCache::GetCacheFileName()
{
	return m_RootPath + "\\cache.db";
}

BOOL CSWFImportCache::TryToOpenFile(CFile &file, const CString &fileName, UINT nOpenFlags)
{
	BOOL bRet = FALSE;
	CFileException e;
	for (int nRep = 0; nRep < 5; nRep++)	
	{
		bRet = file.Open(fileName, nOpenFlags, &e);
		if (bRet)
			break;
		if (e.m_cause == CFileException::fileNotFound)
			nOpenFlags |= CFile::modeCreate;
		else if (e.m_cause != CFileException::accessDenied)
			break;
	}
	return bRet;
}

BOOL CSWFImportCache::FindInCache(const CString &swfFile, CString &cachePath)
{
	CWaitCursor xWait;

	CString cacheFileName = GetCacheFileName();

	CStdioFile cacheFile;
	if (!TryToOpenFile(cacheFile, cacheFileName, CFile::modeReadWrite | CFile::typeBinary))
		return FALSE;

	m_Cache.clear();
	if (ReadCacheTable(cacheFile, m_Cache) != 0)
	{
		cacheFile.Close();
		DeleteFile(cacheFileName);
		return FALSE;
	}
	Cleanup();

	_MyCache::iterator itd = m_Cache.find(swfFile);	
	
	if (itd != m_Cache.end())
	{
		cachePath = (*itd).second.cacheFolder;
		
		GetSystemTime(&(*itd).second.lastAccessTime); // set last access time
		
		try
		{
			cacheFile.SetLength(0); // empty cache file
			cacheFile.SeekToBegin();
		}
		catch (CFileException *e)
		{
			e->Delete();
			return TRUE;
		}

		WriteCacheTable(cacheFile, m_Cache);		

		return TRUE;
	}

	return FALSE;
}

BOOL CSWFImportCache::Import(const CString &swfFile)
{
	m_bResult = FALSE;
	m_pbCanceled = NULL;	
	
	if (m_bCache) // search file from cache
	{
		if (FindInCache(swfFile, m_sSavePath))
		{
			m_bResult = TRUE;
			if (m_pfCallback != NULL)
			{
				m_pfCallback(SWFIMPORT_START, m_lParam);
				m_pfCallback(MAX_STEP, m_lParam);
				m_pfCallback(SWFIMPORT_END, m_lParam);
			}
			return TRUE;
		}			
	}

	GUID guid;
	memset(&guid, 0, sizeof(guid));
	VERIFY(SUCCEEDED(CoCreateGuid(&guid)));
	OLECHAR oszPath[40];
	VERIFY(StringFromGUID2(guid, oszPath, 40) != 0);	
	m_sSavePath.Format("%s\\%S", m_RootPath, oszPath);
	m_sSWFFile = swfFile;

	// create import thread
	ASSERT(m_hThread == NULL);
	
	DWORD threadId = 0;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ImportThreadProc,
		(LPVOID)this, 0, &threadId);
	ASSERT(m_hThread != NULL);	

	return TRUE;	
}

BOOL CSWFImportCache::GetImportResult(CString &sPath)
{
	if (m_hThread != NULL)
	{
		TRACE("Import SWF: Wait Thread Exit\n");
		VERIFY(WaitForSingleObject(m_hThread, INFINITE) == WAIT_OBJECT_0);
		TRACE("Import SWF: Thread Exit\n");
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	
	if (m_bResult)
		sPath = m_sSavePath;

	return m_bResult;
}

DWORD WINAPI CSWFImportCache::ImportThreadProc(LPVOID pThis)
{
	CSWFImportCache *me = (CSWFImportCache *)pThis;
	ASSERT(me);

	CSubject sub;
	CSubjectManager::Instance()->Register("ImportSWF", &sub);
	sub.Attach(me);

	CSWFParse parse;
	me->m_pbCanceled = parse.GetCanceledFlagPtr();
	ASSERT(me->m_pbCanceled != NULL);
	*(me->m_pbCanceled) = false;

	sub.Notify((void *)SWFIMPORT_START);

	if (!me->ImportSWFToPath(me->m_sSWFFile, me->m_sSavePath, &parse, &sub))
	{
		if (PathFileExists(me->m_sSavePath)) // export failed
			RemoveDirectoryRecursively(me->m_sSavePath);
	}

	sub.Notify((void *)SWFIMPORT_END);

	CSubjectManager::Instance()->UnRegister("ImportSWF");

	return 0;
}

BOOL CSWFImportCache::ImportSWFToPath(const CString &swfFile, const CString &sPath, CSWFParse *pParse, CSubject *pSub)
{
	
#define IF_CONTINUE(v) \
if (pSub != NULL) \
{	\
	pSub->Notify((void *)(v));	\
	if (*m_pbCanceled)	\
	{	\
		m_strError = "User canceled.";	\
		return FALSE;	\
	}	\
}

	ASSERT(pParse != NULL);
	
	if (!pParse->ParseFileStructure(swfFile))
	{
		m_strError.Format(IDS_FAILED_IMPORT_s, (LPCTSTR)swfFile);
		return FALSE;
	}

	if (!PrepareDirectory(sPath, FALSE))
	{
		m_strError.Format(IDS_FAILED_CREATE_DIRECTORY_s, (LPCTSTR)sPath);
		return FALSE;
	}

	TRACE("Import SWF: Parsing SWF File\n");

	gldLibrary lib;
	CTransAdaptor::SetAdaptor(&lib);

	gldLibrary *oldLib = gldDataKeeper::Instance()->m_objLib;
	gldDataKeeper::Instance()->m_objLib = &lib;
	gldSWFImporter importer(pParse, &lib);
	gldMovieClip movie;
	bool bRet = importer.ImportSWFMovie(pParse->m_tagList, &movie);
	gldDataKeeper::Instance()->m_objLib = oldLib;	
	if (!bRet)
	{		
		m_strError.Format(IDS_FAILED_IMPORT_s, (LPCTSTR)swfFile);
		return FALSE;
	}

	_ImportInfo info[] = {
		{gobjShape, 0, "", "shapes"},
		{gobjButton, 0, "", "buttons"},
		{gobjSprite, 0, "", "movieclips"},
		{gobjImage, 0, "", "images"}, 
		{gobjSound, 0, "", "sounds"}
	};
	
	TRACE("Import SWF: Prepare Text\n");
	int n = 0;
	const GOBJECT_LIST &lstObj = lib.GetObjList();	
	ITextTool IClass;
	GOBJECT_LIST::const_iterator ito;
	for (ito = lstObj.begin(); ito != lstObj.end(); ++ito, n++)
	{
		gldObj *pObj = (*ito);
		if (pObj->IsGObjInstanceOf(gobjText))
		{
			gldText2 *pText = (gldText2 *)pObj;
			pText->m_bounds = pText->m_bounds;
			SETextToolEx TTool(&IClass);
			TTool.ConvertFromGldText(*pText);
			TTool.ConvertToGldText(*pText);
			CTransAdaptor::UpdateTShape(pText);
		}
		IF_CONTINUE(80 + 20 * n / lstObj.size())		
	}
	
	TRACE("Import SWF: Start Write File\n");
	CCoInit xCoInit;
	if (FAILED(xCoInit.m_Result))
	{
		m_strError.LoadString(IDS_FAILED_INIT_COM);
		return FALSE;
	}
	
	n = 0;
	for (ito = lstObj.begin(); ito != lstObj.end(); ++ito, ++n)
	{
		gldObj *pObj = *ito;
		int i = 0;
		for (; i < sizeof(info) / sizeof(_ImportInfo); i++)
		{
			if (info[i].type == pObj->GetGObjType())
				break;
		}
		if (i < sizeof(info) / sizeof(_ImportInfo))
		{
			if (info[i].path.GetLength() == 0)
			{	
				info[i].path.Format("%s\\%s", (LPCTSTR)sPath, (LPCTSTR)info[i].folder);
				CreateDirectoryNest(info[i].path);
			}			

			if (info[i].type == gobjShape || info[i].type == gobjButton || info[i].type == gobjSprite)
			{
				if (ExportSSObject(pObj, info[i].path))
					info[i].num++;
			}
			else if (info[i].type == gobjSound)
			{
				if (ExportSound((gldSound *)pObj, info[i].path))
					info[i].num++;
			}
			else if (info[i].type == gobjImage)
			{
				if (ExportImage((gldImage *)pObj, info[i].path))
					info[i].num++;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
		IF_CONTINUE(100 + 100 * n / lstObj.size());
	}	
	
	if (m_bCache)
		AddCacheItem(swfFile, sPath);

	m_bResult = TRUE;

	return TRUE;
}

void CSWFImportCache::AddCacheItem(const CString &swfFile, const CString &sPath)
{
	DWORD dwCrc32 = 0;
	if (CCrc32Static::FileCrc32Win32(swfFile, dwCrc32) != ERROR_SUCCESS)
		return;

	CStdioFile cacheFile;
	CString cacheFileName = GetCacheFileName();
	if (!TryToOpenFile(cacheFile, cacheFileName, CFile::modeReadWrite | CFile::typeBinary))
		return;

	m_Cache.clear();
	ReadCacheTable(cacheFile, m_Cache);
	
	_CacheData data;
	data.cacheFolder = sPath;
	data.crc = dwCrc32;
	GetSystemTime(&data.lastAccessTime);
	m_Cache.insert(_MyCache::value_type(swfFile, data));

	Cleanup();

	try
	{
		cacheFile.SetLength(0); // empty cache file
		cacheFile.SeekToBegin();
	}
	catch (CFileException *e)
	{
		e->Delete();
		return;
	}

	WriteCacheTable(cacheFile, m_Cache);	
}

void CSWFImportCache::Update(void *pvData)
{
	if (m_pfCallback != NULL)
	{
		ASSERT(m_pbCanceled != NULL);
		*m_pbCanceled = !m_pfCallback((int)pvData, m_lParam);
	}
}

BOOL CSWFImportCache::Initialize()
{
	m_RootPath = GetCachePath();
	if (!PrepareDirectory(m_RootPath, FALSE))
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, "Failed to create directory %: %s.", 
			(LPCTSTR)m_RootPath, 
			(LPCTSTR)GetLastErrorMessage());
		return FALSE;
	}

	return TRUE;
}

int CSWFImportCache::ReadCacheTable(CFile &file, _MyCache &cache)
{
	if (file.GetLength() > MAX_CACHE_TABLE)
		return 1;

	UINT dwLen = (UINT)file.GetLength();
	if (dwLen < 19)	// "cache" + ver + num of entry + crc32
		return 1;

	CAutoPtr<BYTE> pBuf(new BYTE[dwLen]);
	try
	{
		dwLen = file.Read(pBuf.m_p, dwLen);
	}
	catch (CFileException *e)
	{
		e->Delete();
		return 1;
	}

	DWORD dwCrc32 = 0;
	CCrc32Static::BufferCrc32(pBuf.m_p, dwLen - sizeof(DWORD), dwCrc32);
	if (dwCrc32 != *(DWORD *)(pBuf.m_p + dwLen - sizeof(DWORD)))
		return 1;

	iBinStream is;
	is.ReadFromMemory(dwLen - sizeof(DWORD), pBuf.m_p);
	pBuf.Free();

	string flag;
	is >> flag;
	if (_tcsicmp(flag.c_str(), "cache") != 0)
		return 1;
	WORD ver = 0;
	is >> ver;
	if (ver != 2)
		return 1;

	DWORD dwNum = 0;
	is >> dwNum;
	for (DWORD i = 0; i < dwNum; i++)
	{
		string swfFile, folder;
		is >> swfFile;
		_CacheData data;
		is >> folder;
		data.cacheFolder = folder.c_str();
		is >> data.crc;		
		is >> data.lastAccessTime.wYear;
		is >> data.lastAccessTime.wMonth;
		is >> data.lastAccessTime.wDay;
		is >> data.lastAccessTime.wDayOfWeek;
		is >> data.lastAccessTime.wHour;
		is >> data.lastAccessTime.wMinute;
		is >> data.lastAccessTime.wSecond;
		is >> data.lastAccessTime.wMilliseconds; 
		cache.insert(_MyCache::value_type(swfFile.c_str(), data));
	}

	return 0;
}

int CSWFImportCache::WriteCacheTable(CFile &file, _MyCache &cache)
{
	oBinStream os;
	os << string("Cache"); // flag
	os << (WORD)2; // version
	os << (DWORD)cache.size();
	for (_MyCache::iterator it = cache.begin(); it != cache.end(); ++it)
	{
		os << string((LPCTSTR)(*it).first);	// swf file
		_CacheData &data = (*it).second;
		os << string((LPCTSTR)data.cacheFolder);
		os << data.crc;
		os << data.lastAccessTime.wYear;
		os << data.lastAccessTime.wMonth;
		os << data.lastAccessTime.wDay;
		os << data.lastAccessTime.wDayOfWeek;
		os << data.lastAccessTime.wHour;
		os << data.lastAccessTime.wMinute;
		os << data.lastAccessTime.wSecond;
		os << data.lastAccessTime.wMilliseconds;			
	}
	DWORD dwCrc32 = 0;
	CCrc32Static::BufferCrc32(os.Memory(), (DWORD)os.Size(), dwCrc32);
	os << dwCrc32;

	try
	{
		file.Write(os.Memory(), (UINT)os.Size());
	}
	catch (CFileException *e)
	{
		e->Delete();		
		return -1;
	}

	return 0;
}

void CSWFImportCache::Uninitialize()
{
}

void CSWFImportCache::Cleanup()
{
	_MyCache::iterator i = m_Cache.begin();

	while (i != m_Cache.end())
	{
		bool bRmv = true;
		const CString &swfFile = (*i).first;
		const _CacheData &data = (*i).second;
		
		DWORD dwCrc32 = 0;
		if (CCrc32Static::FileCrc32Win32(swfFile, dwCrc32) == ERROR_SUCCESS)
		{
			bRmv = (dwCrc32 != data.crc);
		}		

		if (!bRmv)
		{
			bRmv = !PathFileExists(data.cacheFolder);
		}

		if (bRmv)
		{
			_MyCache::iterator in = i;
			in++;
			RemoveDirectoryRecursively(data.cacheFolder);
			m_Cache.erase(i);
			i = in;
		}
		else
		{
			i++;
		}
	}

	if (m_Cache.size() >= MAX_CACHE_ITEM)
	{
		_FTSortQueue q;
		for (i = m_Cache.begin(); i != m_Cache.end(); ++i)
			q.insert(i);
		_FTSortQueue::iterator p = q.begin();
		while (m_Cache.size() >= MAX_CACHE_ITEM)
		{
			RemoveDirectoryRecursively((*(*p)).second.cacheFolder);
			m_Cache.erase(*(p++));
		}
	}
}

CString CSWFImportCache::GetCachePath()
{	
	LPCTSTR pszModulePath = GetModuleFilePath();
	CString sPath;
	sPath.Format("%s\\Resource\\Cache", pszModulePath, pszModulePath);
	return sPath;

	/*
	// user select
	if (sPath.GetLength() == 0)
	{
		BROWSEINFO bi;
		memset(&bi, 0, sizeof(bi));
		bi.lpszTitle = "Please select cache path";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != NULL)
		{
			TCHAR pszPath[MAX_PATH];
			if (SHGetPathFromIDList(pidl, pszPath))
				sPath = pszPath;
			CoTaskMemFree(pidl);
		}
	}

	if (sPath.GetLength() > 0)
	{
		if (!PathFileExists(sPath))
			CreateDirectory(sPath, NULL);
		if (!PathFileExists(sPath))
			sPath.Empty();
	}
	*/

	return sPath;
}