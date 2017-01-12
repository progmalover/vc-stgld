#pragma once

#include <stdio.h>

#define MAX_SECTION_SIZE	0x10000000
#define FREE_SECTION_ID		0x00

#define theIniFile CIniFile::Instance()

typedef struct _IniFileHead
{		
	DWORD crc;
	DWORD offset;

} IniFileHead, *IniFileHeadp;

typedef struct _IniSectionHead
{
	DWORD id;	
	DWORD size;
} IniSectionHead, *IniSectionHeadp;

class CIniFile;
class CIniSection
{
public:
	CIniSection(DWORD id);
	virtual ~CIniSection();

	bool LoadFromFile(CIniFile *file, DWORD id = 0);
	void SaveToFile(CIniFile *file);
	
	CString ReadString();
	void WriteString(const CString &str);
	void WriteBlock(const LPVOID lpvData, DWORD size);
	void ReadBlock(LPVOID *lpvData, DWORD &size);

	inline void Seek(DWORD pos)
	{
		m_dwPos = __min(__max(0, pos), m_dwSize);
	}

	inline void Skip(int count)
	{
		int nNewPos = m_dwPos + count;		
		m_dwPos = __min((DWORD)__max(0, nNewPos), m_dwSize);
	}	
	
	inline void Clear()
	{
		m_dwPos = 0;
		m_dwSize = 0;
	}

	// can't write pointer or class type
	template <typename _T>
	void Write(const _T &x)
	{
		if (m_dwPos + sizeof(_T) > m_dwMaxSize)
			ExpandBufferTo(m_dwMaxSize + 256);
		memcpy(m_lpcData + m_dwPos, &x, sizeof(_T));
		m_dwPos += sizeof(_T);
		m_dwSize = __max(m_dwSize, m_dwPos);
	}	
	
	template <typename _T>
	_T Read(const _T &def)
	{
		if (m_dwPos + sizeof(_T) > m_dwSize)
			return def;
		_T x = *(_T *)(m_lpcData + m_dwPos);
		m_dwPos += sizeof(_T);
		return x;
	}

	inline DWORD Size() const
	{
		return m_dwSize;
	}

protected:
	void ExpandBufferTo(DWORD size);

protected:
	BYTE *m_lpcData;
	DWORD m_dwSize;
	DWORD m_dwMaxSize;
	DWORD m_dwPos;
	DWORD m_dwId;
};

class CIniFile
{
public:
	static CIniFile *Instance();	
	static void Release();

	inline bool Ready() const
	{
		return m_FilePtr != NULL;
	}
	bool ReadSection(DWORD dwID, void **lpvData, DWORD &dwCount);
	bool WriteSection(DWORD dwID, const void *lpvData, DWORD dwCount);
	bool RemoveSection(DWORD dwID);
	void HandleErrorFile();

protected:
	CIniFile(void);
	virtual ~CIniFile(void);

	void SaveFileName();
	CString GetFileName();
	bool ParseFile();
	void Initialize();
	void Uninitialize();	

	DWORD FindSection(DWORD dwID, IniSectionHeadp pSecHead);
	DWORD FindFreeSection(DWORD dwCount);
	DWORD GetFileCrc32();

	CString GenIniFileName();
	void GenIniFile();
	void ClearupFile();
	void RemoveUnusedFiles();

	void HandleException();
	
	void SeekIniFile(FILE *fp, DWORD dwOffset, int origin);
	DWORD ReadIniFile(FILE *fp, void *buffer, DWORD count);
	void WriteIniFile(FILE *fp, const void *buffer, DWORD count);

protected:
	FILE *m_FilePtr;
	CString m_FileName;
	IniFileHead m_FileHead;

	static CIniFile *s_Inst;
};
