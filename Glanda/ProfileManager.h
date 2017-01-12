#pragma once
#include <fstream>

#ifdef _DEBUG
#define _LOGFILE(f, s)														\
{																			\
	CTime tm = CTime::GetCurrentTime();										\
	CString str;															\
	str.Format(CString("%04d/%02d/%02d %02d:%02d:%02d ")+s,					\
	tm.GetYear(),	tm.GetMonth(),	tm.GetDay(),							\
	tm.GetHour(),	tm.GetMinute(),	tm.GetSecond());						\
	std::ofstream file(f, std::ios::app | std::ios::end | std::ios::out);	\
	file << (LPCTSTR)str << std::endl;										\
}

#define _LOGFILE1(f, s, s1)													\
{																			\
	CTime tm = CTime::GetCurrentTime();										\
	CString str;															\
	str.Format(CString("%04d/%02d/%02d %02d:%02d:%02d ")+s,					\
	tm.GetYear(),	tm.GetMonth(),	tm.GetDay(),							\
	tm.GetHour(),	tm.GetMinute(),	tm.GetSecond(),							\
	s1);																	\
	std::ofstream file(f, std::ios::app | std::ios::end | std::ios::out);	\
	file << (LPCTSTR)str << std::endl;										\
}

#define _LOGFILE2(f, s, s1, s2)												\
{																			\
	CTime tm = CTime::GetCurrentTime();										\
	CString str;															\
	str.Format(CString("%04d/%02d/%02d %02d:%02d:%02d ")+s,					\
	tm.GetYear(),	tm.GetMonth(),	tm.GetDay(),							\
	tm.GetHour(),	tm.GetMinute(),	tm.GetSecond(),							\
	s1, s2);																\
	std::ofstream file(f, std::ios::app | std::ios::end | std::ios::out);	\
	file << (LPCTSTR)str << std::endl;										\
}

#define _LOGFILE3(f, s, s1, s2, s3)											\
{																			\
	CTime tm = CTime::GetCurrentTime();										\
	CString str;															\
	str.Format(CString("%04d/%02d/%02d %02d:%02d:%02d ")+s,					\
	tm.GetYear(),	tm.GetMonth(),	tm.GetDay(),							\
	tm.GetHour(),	tm.GetMinute(),	tm.GetSecond(),							\
	s1, s2, s3);															\
	std::ofstream file(f, std::ios::app | std::ios::end | std::ios::out);	\
	file << (LPCTSTR)str << std::endl;										\
}
#else
#define _LOGFILE(f, s)
#define _LOGFILE1(f, s, s1)
#define _LOGFILE2(f, s, s1, s2)
#define _LOGFILE3(f, s, s1, s2, s3)
#endif

#define _LOG(s)					_LOGFILE("main.log", s)
#define _LOG1(s, s1)			_LOGFILE1("main.log", s, s1)
#define _LOG2(s, s1, s2)		_LOGFILE2("main.log", s, s1, s2)
#define _LOG3(s, s1, s2, s3)	_LOGFILE3("main.log", s, s1, s2, s3)