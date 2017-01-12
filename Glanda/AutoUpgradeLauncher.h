#pragma once

#include "Registry.h"
#include "Utils.h"

class CAutoUpgradeLauncher
{
public:

	CAutoUpgradeLauncher(void)
	{
	}

	virtual ~CAutoUpgradeLauncher(void)
	{
	}

	static BOOL Launch(LPCTSTR lpszRegPath, BOOL bSchedule)
	{

		CRegistry reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, "Software\\SourceTec\\Sothink Updater", KEY_READ))
		{
			CString strInstallPath;
			if (reg.Read("Install Path", strInstallPath))
			{
				CString strParam;
				strParam.Format(bSchedule ? "\"%s\" -schedule" : "\"%s\"", lpszRegPath);
				if ((ULONGLONG)ShellExecute(NULL, "open", strInstallPath + "\\SothinkUpdater.exe", strParam, NULL, SW_SHOW) >= 32)
				{
					return TRUE;
				}
			}
		}

		if (!bSchedule)
		{
			SetLastError(ERROR_FILE_NOT_FOUND);
			ReportLastError();
		}

		return FALSE;
	}
};
