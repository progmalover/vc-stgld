#pragma once

class CGlandaVersion
{
public:
	CGlandaVersion(void);
	~CGlandaVersion(void);

	static WORD GetCurVersion();
	static WORD GetSWFLibVersion(WORD glandaVer);
	static WORD GetGlandaVersion(WORD swflibVer);
	static BOOL IsValidVersion(WORD glandaVer);
	static BYTE GetSWFLibMajor(WORD swflibVer);
	static BYTE GetSWFLibMinor(WORD swflibVer);
private:
	struct Glanda2SWFLib
	{
		WORD glandaVer;
		WORD swflibVer;
	};

	static Glanda2SWFLib s_verMap[];
	static const WORD s_curVer;
};
