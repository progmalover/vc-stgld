#include "StdAfx.h"
#include ".\glandaversion.h"

const WORD CGlandaVersion::s_curVer = 4;

CGlandaVersion::Glanda2SWFLib CGlandaVersion::s_verMap[] =
{
	{1, 0x0105},
	{2, 0x013D},//1.61
	{4, 0x023C}
};

CGlandaVersion::CGlandaVersion(void)
{
}

CGlandaVersion::~CGlandaVersion(void)
{
}

WORD CGlandaVersion::GetCurVersion()
{
	return s_curVer;
}

WORD CGlandaVersion::GetSWFLibVersion(WORD glandaVer)
{
	for (int i = 0; i < sizeof(s_verMap) / sizeof(s_verMap[0]); i++)
	{
		if (s_verMap[i].glandaVer == glandaVer)
			return s_verMap[i].swflibVer;
	}
	return 0;
}

WORD CGlandaVersion::GetGlandaVersion(WORD swflibVer)
{
	for (int i = 0; i < sizeof(s_verMap) / sizeof(s_verMap[0]); i++)
	{
		if (s_verMap[i].swflibVer == swflibVer)
			return s_verMap[i].glandaVer;
	}
	return 0;
}

BOOL CGlandaVersion::IsValidVersion(WORD glandaVer)
{
	for (int i = 0; i < sizeof(s_verMap) / sizeof(s_verMap[0]); i++)
	{
		if (s_verMap[i].glandaVer == glandaVer)
			return TRUE;
	}
	return FALSE;
}

BYTE CGlandaVersion::GetSWFLibMajor(WORD swflibVer)
{
	return (BYTE)((swflibVer >> 8) & 0xFF);
}


BYTE CGlandaVersion::GetSWFLibMinor(WORD swflibVer)
{
	return (BYTE)(swflibVer & 0xFF);
}