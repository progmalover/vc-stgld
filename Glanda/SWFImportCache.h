#pragma once

#include "Singleton.h"
#include <map>
#include "Observer.h"
using namespace std;

#define SWFIMPORT_START	-1
#define SWFIMPORT_END		-2

typedef BOOL (WINAPI *IMPORTSWFCALLBACK)(int rate, LPARAM lParam);

class CSWFParse;
class CSWFImportCache : public CSingleton<CSWFImportCache>, public CObserver
{
public:
	struct _CacheData
	{
		DWORD crc;
		CString cacheFolder;
		SYSTEMTIME lastAccessTime;		
	};

	typedef map<CString, _CacheData> _MyCache;
	_MyCache m_Cache;
	CString m_RootPath;

	CSWFImportCache(bool bCache = true);
	virtual ~CSWFImportCache();

	void SetCachable(bool bCache) { m_bCache = bCache; }
	CString GetErrorMessage() {return m_strError;}
	BOOL Import(const CString &swfFile);
	BOOL GetImportResult(CString &sPath);
	BOOL ImportSWFToPath(const CString &swfFile, const CString &sPath, CSWFParse *pParse, CSubject *pSub);

	void SetCallback(IMPORTSWFCALLBACK pfCallback, LPARAM lParam) { m_pfCallback = pfCallback; m_lParam = lParam; }
	virtual void Update(void *pvData);

	static DWORD WINAPI ImportThreadProc(LPVOID pThis);
	void AddCacheItem(const CString &swfFile, const CString &sPath);
	BOOL FindInCache(const CString &swfFile, CString &cachePath);	

	BOOL Initialize();
	void Uninitialize();
	void Cleanup();
	static CString GetCachePath();

	enum
	{
		MAX_CACHE_ITEM = 100,
		MAX_STEP = 200,
	};

protected:	
	int ReadCacheTable(CFile &file, _MyCache &cache);
	int WriteCacheTable(CFile &file, _MyCache &cache);
	BOOL TryToOpenFile(CFile &file, const CString &fileName, UINT nOpenFlags);	
	CString GetCacheFileName();

protected:
	IMPORTSWFCALLBACK m_pfCallback;
	LPARAM m_lParam;
	bool *m_pbCanceled;
	HANDLE m_hThread;
	CString m_sSavePath;
	CString m_sSWFFile;
	BOOL m_bResult;	
	CString m_strError;
	bool m_bCache;
};