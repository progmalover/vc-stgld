#pragma once

//#ifndef _QWORD_DEFINED
//#define _QWORD_DEFINED
//typedef __int64 QWORD, *LPQWORD;
//#endif

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) \
	((DWORD)(l))
#define HIDWORD(l) \
	((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096

class CCrc32Static
{
public:
	CCrc32Static();
	virtual ~CCrc32Static();

	static void BufferCrc32(const LPVOID lpvBuf, DWORD dwCount, DWORD &dwCrc32); 
	//static DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32);
	//static DWORD FileCrc32Streams(LPCTSTR szFilename, DWORD &dwCrc32);
	static DWORD FileCrc32Win32(LPCTSTR szFilename, DWORD &dwCrc32);
	//static DWORD FileCrc32Filemap(LPCTSTR szFilename, DWORD &dwCrc32);
	//static DWORD FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32);

protected:
	static bool GetFileSizeQW(const HANDLE hFile, QWORD &qwSize);
	static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);

	static DWORD s_arrdwCrc32Table[256];
};
