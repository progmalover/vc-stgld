#include "StdAfx.h"
#include "inifile.h"
#include "Crc32Static.h"
#include "shlwapi.h"

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CIniFile *CIniFile::s_Inst = NULL;

CIniSection::CIniSection(DWORD id)
{
	ASSERT(id != 0);

	m_dwSize = 0;
	m_dwMaxSize = 256;
	m_lpcData = new BYTE[m_dwMaxSize];
	m_dwPos = 0;
	m_dwId = id;
}

CIniSection::~CIniSection()
{
	if (m_lpcData != NULL)
		delete[] m_lpcData;
}

bool CIniSection::LoadFromFile(CIniFile *file, DWORD id /*= 0*/)
{
	if (m_lpcData != NULL)
		delete[] m_lpcData;

	if (id == 0)
		id = m_dwId;

	ASSERT(id != 0);

	bool bRet = file->ReadSection(id, (void **)&m_lpcData, m_dwSize);
	if (bRet)
	{
		m_dwMaxSize = m_dwSize;
		m_dwPos = 0;
		m_dwId = id;
	}
	else
	{
		m_lpcData = NULL;
		m_dwSize = 0;
		m_dwMaxSize = 0;
		m_dwPos = 0;
		m_dwId = id;
	}

	return bRet;
}

void CIniSection::ReadBlock(LPVOID *lpvData, DWORD &size)
{
	ASSERT(lpvData != NULL);
	size = __min(m_dwSize - m_dwPos, size);
	*lpvData = m_lpcData + m_dwPos;
	m_dwPos += size;
}

void CIniSection::WriteBlock(const LPVOID lpvData, DWORD size)
{
	if (m_dwPos + size > m_dwMaxSize)
		ExpandBufferTo(m_dwPos + size);
	memcpy(m_lpcData + m_dwPos, lpvData, size);
	m_dwPos += size;
	m_dwSize += size;
}

CString CIniSection::ReadString()
{
	DWORD sLen = 0;
	CString str;

	if ((sLen = Read(sLen)) != 0)
	{		
		if (m_dwPos + sLen > m_dwSize)
			m_dwPos -= sizeof(DWORD);
		else
		{
			char *psz = (char *)(m_lpcData + m_dwPos);
			for (DWORD i = 0; i < sLen; i++)
				str.Insert(i, *(psz++));
			m_dwPos += sLen;
		}			
	}

	return str;
}

void CIniSection::WriteString(const CString &str)
{
	DWORD sLen = str.GetLength();
	Write(sLen);
	WriteBlock((const LPVOID)((const char *)str), sLen * sizeof(char));
}

void CIniSection::SaveToFile(CIniFile *file)
{	
	file->WriteSection(m_dwId, m_lpcData, m_dwSize);
}

void CIniSection::ExpandBufferTo(DWORD size)
{		
	ASSERT(size > m_dwMaxSize);

	m_dwMaxSize = size;
	BYTE *lpcData = new BYTE[m_dwMaxSize];
	if (m_lpcData != NULL)
	{
		memcpy(lpcData, m_lpcData, m_dwSize);
		delete[] m_lpcData;
	}
	m_lpcData = lpcData;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

CIniFile::CIniFile(void)
{	
	RemoveUnusedFiles();

	Initialize();
}

CIniFile::~CIniFile(void)
{
	Uninitialize();
}

CIniFile *CIniFile::Instance()
{
	if (s_Inst == NULL)			
		s_Inst = new CIniFile;

	return s_Inst;
}

void CIniFile::Release()
{
	if (s_Inst != NULL)
		delete s_Inst;
}

DWORD CIniFile::FindSection(DWORD dwID, IniSectionHeadp pSecHead)
{	
	ASSERT(pSecHead != NULL);

	DWORD dwOffset = m_FileHead.offset;
	DWORD dwNumOfRead = 0;

	try
	{	
		while (true)
		{
			SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);
			
			dwNumOfRead = ReadIniFile(m_FilePtr, pSecHead, sizeof(IniSectionHead));

			if (dwNumOfRead != sizeof(IniSectionHead) || pSecHead->id == dwID)
				break;

			dwOffset += pSecHead->size + sizeof(IniSectionHead);
		}
		
		if (dwNumOfRead == sizeof(IniSectionHead))
		{
			ASSERT(pSecHead->id == dwID);
			if (pSecHead->size >= 0 && pSecHead->size <= MAX_SECTION_SIZE)
				return dwOffset;
			else
				HandleErrorFile();
		}
		else if (dwNumOfRead > 0)	// file format error
		{
			ASSERT(dwNumOfRead < sizeof(IniSectionHead));
			HandleErrorFile();		
		}
	}
	catch(...)
	{
		HandleException();

		return 0;
	}

	return 0;
}

DWORD CIniFile::FindFreeSection(DWORD dwCount)
{
	IniSectionHead SecHead;
	DWORD dwOffset = m_FileHead.offset;
	DWORD dwNumOfRead = 0;

	try
	{
		while (true)
		{	
			SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);
			
			dwNumOfRead = ReadIniFile(m_FilePtr, &SecHead, sizeof(SecHead));
			
			if (dwNumOfRead != sizeof(SecHead)
				|| (SecHead.id == FREE_SECTION_ID && SecHead.size == dwCount))		
				break;
			
			dwOffset += SecHead.size + sizeof(IniSectionHead);
		}
		
		if (dwNumOfRead == sizeof(IniSectionHead))
		{
			ASSERT(SecHead.id == FREE_SECTION_ID);

			return dwOffset;
		}
		else if (dwNumOfRead > 0)	// file format error
		{
			ASSERT(dwNumOfRead < sizeof(IniSectionHead));
			HandleErrorFile();		
		}
	}
	catch(...)
	{
		HandleException();

		return 0;
	}

	return 0;
}

bool CIniFile::ReadSection(DWORD dwID, void **lpvData, DWORD &dwCount)
{	
	if (m_FilePtr == NULL)
		return false;
	
	IniSectionHead SecHead;
	DWORD dwOffset;
	void *lpvSec = NULL;

	try
	{
		dwOffset = FindSection(dwID, &SecHead);

		if (dwOffset != 0)
		{
			if (SecHead.size == 0)
			{
				*lpvData = NULL;
				dwCount = 0;
				return true;
			}
			
			lpvSec = new unsigned char[SecHead.size];
			
			SeekIniFile(m_FilePtr, dwOffset + sizeof(IniSectionHead), SEEK_SET);
			
			if (ReadIniFile(m_FilePtr, lpvSec, SecHead.size) != SecHead.size)
			{	// file format error
				delete[] lpvSec;
				HandleErrorFile();
				return false;				
			}
			else
			{	// return the section
				ASSERT(lpvData != NULL);
				*lpvData = lpvSec;
				dwCount = SecHead.size;
				return true;
			}
		}
		else	// the section not found
			return false;
	}
	catch(...)
	{
		if (lpvSec != NULL)
			delete[] lpvSec;

		HandleException();

		return false;
	}	
}

bool CIniFile::WriteSection(DWORD dwID, const void *lpvData, DWORD dwCount)
{
	if (m_FilePtr == NULL)
		return false;
	
	IniSectionHead SecHead;
	DWORD dwOffset;

	try
	{
		dwOffset = FindSection(dwID, &SecHead);

		if (dwOffset != 0)
		{
			if (SecHead.size == dwCount)
				SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);				
			else
			{	// free section
				SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);				
				SecHead.id = FREE_SECTION_ID;
				WriteIniFile(m_FilePtr, &SecHead, sizeof(SecHead));
				SeekIniFile(m_FilePtr, 0, SEEK_END);
			}
		}
		else
		{
			dwOffset = FindFreeSection(dwCount);
			if (dwOffset == 0)
				SeekIniFile(m_FilePtr, 0, SEEK_END);
			else
				SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);
		}

		SecHead.id = dwID;
		SecHead.size = dwCount;
		WriteIniFile(m_FilePtr, &SecHead, sizeof(SecHead));
		WriteIniFile(m_FilePtr, lpvData, dwCount);		
	}
	catch(...)
	{
		HandleException();

		return false;
	}	
	
	return true;
}

bool CIniFile::RemoveSection(DWORD dwID)
{
	if (m_FilePtr == NULL)
		return false;
	
	IniSectionHead SecHead;
	DWORD dwOffset = 0;

	try
	{
		dwOffset = FindSection(dwID, &SecHead);

		if (dwOffset != 0)
		{
			SeekIniFile(m_FilePtr, dwOffset, SEEK_SET);
			
			SecHead.id = FREE_SECTION_ID;

			WriteIniFile(m_FilePtr, &SecHead, sizeof(SecHead));	
		}
	}
	catch(...)
	{
		HandleException();

		return false;
	}

	return dwOffset != 0;
}

void CIniFile::SaveFileName()
{
	AfxGetApp()->WriteProfileString("User Config", 
		"INI File", m_FileName);
}

CString CIniFile::GetFileName()
{
	return AfxGetApp()->GetProfileString("User Config", 
		"INI File", "");
}

DWORD CIniFile::GetFileCrc32()
{
	BYTE buffer[1024];
	DWORD dwNumOfRead = 0;
	DWORD dwCrc32 = 0;

	SeekIniFile(m_FilePtr, m_FileHead.offset, SEEK_SET);
	
	do
	{
		dwNumOfRead = ReadIniFile(m_FilePtr, buffer, 1024);
		CCrc32Static::BufferCrc32(buffer, dwNumOfRead, dwCrc32);
	} while (dwNumOfRead > 0);
	
	return dwCrc32;
}

bool CIniFile::ParseFile()
{
	DWORD dwNumOfRead;

	SeekIniFile(m_FilePtr, 0, SEEK_SET);
	dwNumOfRead = ReadIniFile(m_FilePtr, &m_FileHead, sizeof(m_FileHead));

	return dwNumOfRead == sizeof(m_FileHead) 		
		&& m_FileHead.offset == sizeof(m_FileHead)
		&& m_FileHead.crc == GetFileCrc32();
}

CString CIniFile::GenIniFileName()
{
	char sModulePath[MAX_PATH + 1];	
	size_t nModulePath;
	VERIFY((nModulePath = GetModuleFileName(
		AfxGetInstanceHandle(), 
		sModulePath, MAX_PATH)) != 0);
	char *p = sModulePath + nModulePath - 1;
	while (p > sModulePath && *p != '\\')	
		*(p--) = '\0';

	CString IniFileName;
	IniFileName.Format("%suser.cfg", sModulePath);
	int Count = 0;
	while (PathFileExists(IniFileName))	
		IniFileName.Format("%suser%d.cfg", sModulePath, Count++);
		
	return IniFileName;
}

void CIniFile::Initialize()
{	
	m_FileName = GetFileName();

	try
	{
		if (m_FileName.GetLength() == 0 || !PathFileExists(m_FileName))
			GenIniFile();
		else
		{
			m_FilePtr = fopen(m_FileName, "r+b");

			if (m_FilePtr == NULL)
				GenIniFile();
			else
			{
				if (!ParseFile())
					HandleErrorFile();
				else
					ClearupFile();
			}
		}
	}
	catch(...)
	{
		HandleException();
	}
}

void CIniFile::GenIniFile()
{
	m_FileName = GenIniFileName();
	m_FilePtr = fopen(m_FileName, "w+b");
	if (m_FilePtr != NULL)
	{		
		m_FileHead.crc = 0;
		m_FileHead.offset = sizeof(m_FileHead);	
		WriteIniFile(m_FilePtr, &m_FileHead, sizeof(m_FileHead));
	}
}

void CIniFile::Uninitialize()
{
	if (m_FilePtr == NULL)
		return;	

	try
	{
		// update crc	
		m_FileHead.crc = GetFileCrc32();
		m_FileHead.offset = sizeof(m_FileHead);
		SeekIniFile(m_FilePtr, 0, SEEK_SET);
		WriteIniFile(m_FilePtr, &m_FileHead, sizeof(m_FileHead));
		// close file
		fclose(m_FilePtr);

		CString sFileName = GetFileName();
		// initialize open file failed then try to remove it		
		if (sFileName.CompareNoCase(m_FileName) != 0 && PathFileExists(sFileName))
			DeleteFile(sFileName);

		// save ini file to registry
		SaveFileName();
	}
	catch(...)
	{
		HandleException();
	}
}

void CIniFile::HandleErrorFile()
{
	if (m_FilePtr != NULL)
	{
		// remove error file
		fclose(m_FilePtr);
		DeleteFile(m_FileName);
	}
	
	GenIniFile();
}

void CIniFile::ClearupFile()
{
	CString TmpFileName = GenIniFileName();

	FILE *TmpFilePtr = NULL;
	BYTE buffer[1024];	
	DWORD NumOfRead, NumOfReadSection;
	IniSectionHead SecHead;

	try
	{
		TmpFilePtr = fopen(TmpFileName, "w+b");
		if (TmpFilePtr == NULL)
			return;

		IniFileHead	FileHead;
		FileHead.crc = 0;
		FileHead.offset = sizeof(FileHead);
		WriteIniFile(TmpFilePtr, &FileHead, sizeof(FileHead));

		SeekIniFile(m_FilePtr, m_FileHead.offset, SEEK_SET);
		while (true)
		{
			NumOfRead = ReadIniFile(m_FilePtr, &SecHead, sizeof(SecHead));
			if (NumOfRead == 0)	// end of file
				break;
			else if (NumOfRead != sizeof(SecHead))	// error file format
			{
				fclose(TmpFilePtr);
				DeleteFile(TmpFileName);
				HandleErrorFile();
				return;
			}

			if (SecHead.id != FREE_SECTION_ID)
			{
				// write section header
				WriteIniFile(TmpFilePtr, &SecHead, sizeof(SecHead));

				NumOfReadSection = 0;
				while (NumOfReadSection < SecHead.size)
				{
					NumOfRead = __min(1024, SecHead.size - NumOfReadSection);
					NumOfRead = ReadIniFile(m_FilePtr, buffer, NumOfRead);
					if (NumOfRead == 0)	// end of file
						break;
					WriteIniFile(TmpFilePtr, buffer, NumOfRead);
					NumOfReadSection += NumOfRead;			
				}
				if (NumOfReadSection != SecHead.size)
				{
					fclose(TmpFilePtr);
					DeleteFile(TmpFileName);
					HandleErrorFile();
					return;
				}
			}
			else
			{
				SeekIniFile(m_FilePtr, SecHead.size, SEEK_CUR);
			}
		}
		fclose(m_FilePtr);
		DeleteFile(m_FileName);
		m_FileName = TmpFileName;
		m_FilePtr = TmpFilePtr;
	}
	catch(...)
	{
		if (TmpFilePtr != NULL)
			fclose(TmpFilePtr);
		DeleteFile(TmpFileName);
	}
}

void CIniFile::HandleException()
{	
	fclose(m_FilePtr);
	m_FilePtr = NULL;	
	DeleteFile(m_FileName);

	TRACE("Error: Exception occured in ini file I/O.\n");
}

void CIniFile::SeekIniFile(FILE *fp, DWORD dwOffset, int origin)
{
	if (fseek(fp, dwOffset, origin) != 0)
		throw 1;
}

DWORD CIniFile::ReadIniFile(FILE *fp, void *buffer, DWORD count)
{
	DWORD dwRead = 0;

	if ((dwRead = (DWORD)fread(buffer, 1, count, fp)) != count
		&& ferror(m_FilePtr))
		throw 1;

	return dwRead;
}

void CIniFile::WriteIniFile(FILE *fp, const void *buffer, DWORD count)
{
	if (fwrite(buffer, 1, count, fp) != count)
		throw 1;
}

void CIniFile::RemoveUnusedFiles()
{
	// get program path
	char sModulePath[MAX_PATH + 1];	
	size_t nModulePath;
	VERIFY((nModulePath = GetModuleFileName(
		AfxGetInstanceHandle(), 
		sModulePath, MAX_PATH)) != 0);
	char *p = sModulePath + nModulePath - 1;
	while (p > sModulePath && *p != '\\')	
		*(p--) = '\0';
	// current used ini file
	CString sUsedFileName = GetFileName();	
	// find all cfg file in program directory	
	CString sFindFileName;
	CString sFileName;
	sFindFileName.Format("%s*.cfg", sModulePath);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(sFindFileName, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			sFileName.Format("%s%s", sModulePath, FindFileData.cFileName);			
			if (sFileName.CompareNoCase(sUsedFileName) != 0)	// is unused ini file
				DeleteFile(sFileName);
		} while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
}