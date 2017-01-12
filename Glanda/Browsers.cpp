// Browsers.cpp: implementation of the CBrowsers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "Browsers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(CBrowsers)

CBrowsers::CBrowsers()
{
	m_nBrowserNum = 0;
	InitBrowserList();
}

CBrowsers::~CBrowsers()
{
	// Release browser list

}

////////////////////////////////////////////////////////////
//
//	This function get the installed browser's count
//
//	Returned Values:
//		The count of the installed browsers
//
////////////////////////////////////////////////////////////

UINT CBrowsers::GetBrowserNum()
{
	return m_nBrowserNum;
}

////////////////////////////////////////////////////////////
//
//	This function get the installed browser's name
//
//	Parameters:
//		NameIndex:		the browser's index 
//
//	Returned Values:
//		The name of the installed browser
//
////////////////////////////////////////////////////////////

CString CBrowsers::GetBrowserName(int NameIndex)
{
	int size=saBrowserName.GetSize();
	
	if (NameIndex>=0 && NameIndex<size){
		return saBrowserName.GetAt(NameIndex);
	}else{
		return "";
	}
}

////////////////////////////////////////////////////////////
//
//	This function get the installed browser's full path
//
//	Parameters:
//		ExeIndex:		the browser's index 
//
//	Returned Values:
//		The full path of the installed browser
//
////////////////////////////////////////////////////////////

CString CBrowsers::GetBrowserExeFile(int ExeIndex)
{
	int size=saBrowserExeFile.GetSize();
	if (ExeIndex>=0 && ExeIndex<size){
		return saBrowserExeFile.GetAt(ExeIndex);
	}else{
		return "";
	}
}

////////////////////////////////////////////////////////////
//
//	This function get the user specified browser's count
//
//	Returned Values:
//		The count of the user specified browser
//
////////////////////////////////////////////////////////////

UINT CBrowsers::GetSpecifyBrowserNum()
{
	return m_SpecifyNum;
}

////////////////////////////////////////////////////////////
//
//	This function get the specified browser's name
//
//	Parameters:
//		NameIndex:		the browser's index 
//
//	Returned Values:
//		The name of the specified browser
//
////////////////////////////////////////////////////////////

CString CBrowsers::GetSpecifyBrowserName(int NameIndex)
{
	int size=m_SpecifyName.GetSize();
	
	if (NameIndex>=0 && NameIndex<size){
		return m_SpecifyName.GetAt(NameIndex);
	}else{
		return "";
	}
}

////////////////////////////////////////////////////////////
//
//	This function get the specified browser's full path
//
//	Parameters:
//		ExeIndex:		the browser's index 
//
//	Returned Values:
//		The full path of the specified browser
//
////////////////////////////////////////////////////////////

CString CBrowsers::GetSpecifyBrowserExeFile(int ExeIndex)
{
	int size=m_SpecifyPath.GetSize();
	if (ExeIndex>=0 && ExeIndex<size){
		return m_SpecifyPath.GetAt(ExeIndex);
	}else{
		return "";
	}
}

////////////////////////////////////////////////////////////
//
//	This function add a specified browser
//
//	Parameters:
//		Path:		the full path of the specified browser
//		ExeName:	the name of the specified browser
//
//	Returned Values:
//		TURE:		Add operation Success
//		FALSE:		Add operation Failure
//
////////////////////////////////////////////////////////////

BOOL CBrowsers::AddSpecifyBrowser(const CString & Path, 
								  const CString & ExeName)
{
	if (ExeName == ""){
		AfxMessageBox(IDS_ENTER_BROWSER_NAME);
		return FALSE;
	}
	//
	// find a same Exename, if found, return error
	//
	CWinApp* pApp=AfxGetApp();
	int nNum = atoi(pApp->GetProfileString("Customized Browsers", 
		                                   "Count",
											 NULL));	
	CString strTemp,strPos,strDesPos;
	BOOL bHavePos = FALSE;
	int i = 1;
	for (; i <= nNum; i++){
		strPos.Format("%d",i);    
		strTemp = pApp->GetProfileString("Customized Browsers", 
										  "Name"+strPos,NULL);
		if  (strTemp == ExeName)
		{
			AfxMessageBox(IDS_ENTER_BROWSER_NEW_NAME);
			return FALSE;
		}

		if (strTemp == ""){
			bHavePos = TRUE;
			strDesPos = strPos;
		}
	}
	//
	// add to registry
	//
	if (!bHavePos){
		strDesPos.Format("%d",i);
		strPos.Format("%d",i);
	}

	if (SetRegistryString("Customized Browsers","Count",strPos) &&
		SetRegistryString("Customized Browsers","Path" + strDesPos,Path) &&
		SetRegistryString("Customized Browsers","Name" + strDesPos,ExeName)){
		GetSpecifyBrowser();
		return TRUE;
	}else{
		return FALSE;
	}
}

////////////////////////////////////////////////////////////
//
//	This function delete a specified browser according to name
//
//	Parameters:
//		ExeName:	the name of the specified browser
//
//	Returned Values:
//		TURE:		delete operation Success
//		FALSE:		delete operation Failure
//
////////////////////////////////////////////////////////////

BOOL CBrowsers::DelSpecifyBrowser(const CString & ExeName)
{
	CWinApp* pApp=AfxGetApp();
	//
	// check if exist specified browsers
	//
	int nNum = atoi(pApp->GetProfileString("Customized Browsers", 
										"Count",NULL));
	if (nNum <= 0){ 
		return FALSE;
	}
	//
	// find the browser in reg
	//
	CString strTemp,strRes;
	int i=1;
	for (; i <= nNum;i++)
	{      
		strTemp.Format("%d",i);
 
		strRes = pApp->GetProfileString("Customized Browsers", 
									  "Name"+strTemp,NULL);
		if (ExeName == strRes) break;
	}
	if (i > nNum) return FALSE;
	//
	// set the found key value empty
	//
	if (SetRegistryString("Customized Browsers","Path"+strTemp,"") &&
		SetRegistryString("Customized Browsers","Name"+strTemp,"")){
		GetSpecifyBrowser();
		return TRUE;
	}else{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Private Functions

CString CBrowsers::GetIEExeFileName()
{
	CString sPath = GetRegistryString(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 
		"");
	// if not exsit, leave it
	if (sPath == "")
		return "";

	// Add Exe file name
	// sPath = sPath + "\\iexplore.exe";
	return sPath;
}

CString CBrowsers::GetOperaExeFileName()
{
	CString sPath = GetRegistryString(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\windows\\CurrentVersion\\App Paths\\Opera.EXE", 
		"");
	// if not exsit, leave it
	if (sPath == "")
		return "";

	// Add Exe file name
	// sPath = sPath + "\\iexplore.exe";
	return sPath;
}

CString CBrowsers::GetHotJavaExeFileName()
{
	CString sPath = GetRegistryString(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\windows\\CurrentVersion\\App Paths\\HotJava.EXE", 
		"");
	// if not exsit, leave it
	if (sPath == "")
		return "";

	// Add Exe file name
	// sPath = sPath + "\\iexplore.exe";
	return sPath;
}

void CBrowsers::GetSpecifyBrowser()
{
	m_SpecifyNum = 0;
	m_SpecifyPath.RemoveAll();
	m_SpecifyName.RemoveAll();

	CWinApp* pApp=AfxGetApp();
	int Num = atoi(pApp->GetProfileString("Customized Browsers", 
								  "Count",NULL));
	if (Num <= 0) return;
	//
	// initialize the array according to registry
	//
	CString strspecifyPath,strspecifyName;
	CString strTemp;

	for (int i=1; i<=Num;i++)
	{
		strTemp.Format("%d",i);
		strspecifyPath= pApp->GetProfileString("Customized Browsers", 
											   "Path" + strTemp,
											   NULL);
		strspecifyName=pApp->GetProfileString("Customized Browsers", 
											"Name" + strTemp,
											 NULL);
		if (strspecifyPath!="" &&  strspecifyName!="" )
		{ 
			m_SpecifyPath.Add(strspecifyPath);
			m_SpecifyName.Add(strspecifyName);
			m_SpecifyNum ++;
		}
	}
}

CString CBrowsers::GetRegistryString(HKEY hKey,
					  const CString& Path,
					  const CString& ValueName)
////////////////////////////////////////////////////////////
//
//	This function query a string value in registry
//	Parameters:
//		hkey	:	the main Key
//					Identifies a currently open key or any of the 
//					following predefined reserved handle values: 
//						HKEY_CLASSES_ROOT
//						HKEY_CURRENT_USER
//						HKEY_LOCAL_MACHINE
//						HKEY_USERS
//		Path	:	Path of the subKey
//		ValueName	:	Value Name desired
//
//	Returned Values:
//		The value string if success
//		"" if failure or the value not exsit
//
//	Remarks:
//		The max string Length this fuction can retrive is 511
//		TODO : Query a string of any length
//
////////////////////////////////////////////////////////////
{
	HKEY theKey;
	DWORD retValue;
	DWORD valueSize = 512;
	CHAR lpbuffer[512] = "";

	retValue = RegOpenKeyEx(hKey, 
		Path, 
		0, 
		KEY_ALL_ACCESS, 
		&theKey);

	if (ERROR_SUCCESS != retValue)			// Cannot open Registry
		return "";

	retValue = RegQueryValueEx(theKey, 
		(LPTSTR)((LPCSTR)ValueName),
		NULL, 
		NULL, 
		(LPBYTE)lpbuffer, 
		&valueSize);
	
	if (ERROR_SUCCESS != retValue)			// Cannot Query
	{
		// Get the Error description
		LPVOID lpMsgBuf;

		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			retValue,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		// Display the string.
		// MessageBox( NULL, (LPCSTR)lpMsgBuf, "Error Message", MB_OK|MB_ICONINFORMATION );
		TRACE("%s\n", lpMsgBuf);

		// Free the buffer.
		LocalFree( lpMsgBuf );

		RegCloseKey(theKey);
		return "";
	}
	
	RegCloseKey(theKey);
	CString sResult = lpbuffer;
	return sResult;
}

void CBrowsers::InitBrowserList()
{
	CString sFile;

	// Get IE4 file name
	sFile = GetIEExeFileName();
	if (sFile != "")		// If exist
	{
		m_nBrowserNum++;
		saBrowserExeFile.Add(sFile);
		saBrowserName.Add("Internet Explorer");
	}

	// Get Netscape 4 current file name
	sFile = GetNetscape4ExeFileName();
	if (sFile != "")		// If exist
	{
		m_nBrowserNum++;
		saBrowserExeFile.Add(sFile);
		m_Current.TrimRight(' ');
		saBrowserName.Add("Netscape Communicator "+m_Current);
	}
 //Get NetScape 4.0-4.9 file 
	int h=m_Net45Name.GetSize();
    for(int n=0;n<=9;n++)
	 { CString strTemp;

	  if (h!=0)
	  {strTemp=m_Net45Name.GetAt(n);}
	  else
	  {strTemp="empty";}

	   CString strVer;
	   CString strVersion;
	    int l=n;
	 if (strTemp!="empty"){
	  
	   char buffer[10];
       _itoa(l ,buffer, 10);
       strVer=(*buffer);
	  strVersion="4."+strVer;
	 }
	 if (strTemp!="empty" && m_Current!=strVersion)
		{
		m_nBrowserNum++;
		saBrowserExeFile.Add(strTemp);
		saBrowserName.Add("Netscape Communicator "+strVersion);}
	}
 
 // Get Netscape 4.01-4.09 file name
	int q=m_NetScaName.GetSize();
	 for(int j=0;j<9;j++)
	 { CString strTemp;

	  if (q!=0)
	  {strTemp=m_NetScaName.GetAt(j);}
	  else
	  {strTemp="empty";}

	   CString strVer;
	   CString strVersion;
	    int l=j;
		l+=1;
	 if (strTemp!="empty"){
	  
	   char buffer[10];
       _itoa(l ,buffer, 10);
       strVer=(*buffer);
	  strVersion="4.0"+strVer;
	 }
	 if (strTemp!="empty" && m_Current!=strVersion)
		{
		m_nBrowserNum++;
		saBrowserExeFile.Add(strTemp);
		saBrowserName.Add("Netscape Communicator "+strVersion);}
	}

	// Get Netscape 3 file name
	sFile = GetNetscape3ExeFileName();
	if (sFile != "")		// If exist
	{
		m_nBrowserNum++;
		saBrowserExeFile.Add(sFile);
		saBrowserName.Add("Netscape Navigator 3");}

	// Get Opera file name
	sFile = GetOperaExeFileName();
	if (sFile != "")		// If exist
	{
		m_nBrowserNum++;
		saBrowserExeFile.Add(sFile);
		saBrowserName.Add("Opera");
	}

	// Get HotJava file name
	sFile = GetHotJavaExeFileName();
	if (sFile != "")		// If exist
	{
		m_nBrowserNum++;
		saBrowserExeFile.Add(sFile);
		saBrowserName.Add("HotJava");
	}

    GetSpecifyBrowser();
}

CString CBrowsers::GetNetscape4ExeFileName()
{
	//find 4.0-4.9 netsacpe browser
	int v;

	for (int u=0; u<=9; u++)
	{  
	   v=u;
      char buffer[10];
      _itoa(v ,buffer, 10);
      CString str = buffer;
	  str.TrimRight();
	  str.TrimLeft();
	  CString strTrmp="SOFTWARE\\Netscape\\Netscape Navigator\\4."+str+" (en)\\Main";
	  CString strRes=GetRegistryString(HKEY_LOCAL_MACHINE, strTrmp, "Install Directory");
     
	  if  (strRes!=""){
		   strRes=strRes+"\\Program\\Netscape.exe";
	   m_Net45Name.Add(strRes);}
	   else{
		   m_Net45Name.Add("empty");}
     
	}

   //Get 4.01-4.09 Netscape Navigator
	 int m;
    for (int i=0; i<9; i++)
	{ 
	   m=i+1;
      char buffer[10];
      _itoa(m ,buffer, 10);
      CString str= buffer;
	  str.TrimRight();
	  str.TrimLeft();
	  CString strTrmp="SOFTWARE\\Netscape\\Netscape Navigator\\4.0"+str+" (en)\\Main";
	  CString strRes=GetRegistryString(HKEY_LOCAL_MACHINE, strTrmp, "Install Directory");
     
	  if  (strRes!=""){
		   strRes=strRes+"\\Program\\Netscape.exe";
	   m_NetScaName.Add(strRes);}
	   else{
		   m_NetScaName.Add("empty");}
     
	}

   //Get Current Version
	CString sVersion = GetRegistryString(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Netscape\\Netscape Navigator", 
		"CurrentVersion");
	// not exsit, leave blank
	if (sVersion == "")
		return "";
  
	// Get Path
	CString sPath = GetRegistryString(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Netscape\\Netscape Navigator\\" + sVersion + "\\Main", 
		"Install Directory");
	// Add Exe file name
	sPath = sPath + "\\Program\\Netscape.exe";
	int t=sVersion.Find('(');
	if(t > 0)
		m_Current=sVersion.Left(t-1);
	else
		m_Current=sVersion;

	return sPath;

}

CString CBrowsers::GetNetscape3ExeFileName()
{
	//CString NetSc3Exist=GetRegistryString(HKEY_LOCAL_MACHINE, 
	//	"SOFTWARE\\Netscape\\Netscape Navigator Gold", 
	//	"");
	CString sPath = GetRegistryString(HKEY_CURRENT_USER, 
		"SOFTWARE\\Netscape\\Netscape Navigator\\Main", 
		"Install Directory");
	CString sGold = GetRegistryString(HKEY_CURRENT_USER, 
		"SOFTWARE\\Netscape\\Netscape Navigator\\Main", 
		"Mozilla");
	int n=sGold.Find("3.0");
	// not exsit, leave blank
	if (sPath =="" || n==-1)
		return "";
    
	// Add Exe file name
  sPath = sPath + "\\Program\\Netscape.exe";
	CFileFind f;
	if (f.FindFile(sPath)){
        return sPath;
	}
	else
		return"";
}

BOOL CBrowsers::SetRegistryString(const CString & Path,
					              const CString & ValueName,
					              const CString & Value)
{
	int res=AfxGetApp()->WriteProfileString(Path,ValueName,Value);

	return (res==0)?FALSE:TRUE;
}





