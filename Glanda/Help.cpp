#include "stdafx.h"
#include "resource.h"
#include "help.h"
#include "filepath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment (lib, "htmlhelp.lib")

const _HELP_INFO g_HelpInfo[]	= 
{
	{0,	"index.htm"},

	{IDD_PUBLISH_SELECT, "publish/intro.htm"}, 
	{IDD_PUBLISH_EXPORT, "publish/export.htm"},
	{IDD_PUBLISH_HTML_OPTIONS, "publish/htmloptions.htm"}, 
	{IDD_PUBLISH_SELECT_HTML, "publish/insert.htm"}, 
	{IDD_PUBLISH_VIEW_CODE, "publish/copy.htm"}, 


	{IDD_SELECT_EFFECT, "effect/selecteffect.htm"}, 
	
	{IDD_EDIT_GENERAL_PAGE, "dialog/p_general.htm"},
	{IDD_EDIT_SHAPE_FILL_PAGE, "dialog/p_fill.htm"}, 
	{IDD_EDIT_SHAPE_LINE_PAGE, "dialog/p_line.htm"}, 
	{IDD_EDIT_MC_SHAPE_PAGE, "dialog/p_shape.htm"}, 
	{IDD_EDIT_MC_SOUND_PAGE, "dialog/p_sound.htm"}, 
	{IDD_EDIT_MC_TEXT_PAGE, "dialog/p_text.htm"}, 
	{IDD_EDIT_MATRIX_PAGE, "dialog/p_position.htm"}, 
	{IDD_EDIT_CXFORM_PAGE, "dialog/p_color.htm"}, 

	{IDD_SCENE_PROPERTIES_PAGE, "dialog/scene.htm"}, 
	{IDD_SCENE_ACTION_PAGE, "dialog/scene.htm"}, 

	{IDD_MOVIE_PROPERTIES, "dialog/movie.htm"}, 
	{IDD_MOVIE_SOUND, "dialog/movie.htm"}, 

	{IDD_INSTANCE_ACTION_PAGE, "dialog/action.htm"}, 

	{IDD_OPTIONS_GENERAL, "dialog/options.htm"}, 
	{IDD_OPTIONS_TIMELINE, "dialog/options.htm"}, 
	{IDD_OPTIONS_DESIGN, "dialog/options.htm"}, 
	{IDD_OPTIONS_AS, "dialog/options.htm"}, 
	{IDD_OPTIONS_PREVIEW, "dialog/options.htm"}, 

	{IDD_EDIT_GRID, "dialog/grid.htm"}, 
	{IDD_IMPORT_IMAGE_DIALOG, "dialog/importimage.htm"}, 
};

const int g_cHelpInfo			= sizeof(g_HelpInfo) / sizeof(_HELP_INFO);

const TCHAR g_szHelpFilePrefix[]	= "\\Glanda.chm::/";

LPCTSTR GetHelpFileOfID(UINT nID)
{
	for (int i = 0; i < g_cHelpInfo; i ++)
		if (g_HelpInfo[i].nID == nID)
			return g_HelpInfo[i].pszFile;

	// return default page
	return g_HelpInfo[0].pszFile;
}

BOOL ShowHelpFile(LPCTSTR lpszFile)
{
	CString strPath = GetModuleFilePath();
	if (!strPath.IsEmpty())
	{
		strPath += g_szHelpFilePrefix;
		strPath += lpszFile;
		if (!::HtmlHelp(NULL, strPath, HH_DISPLAY_TOPIC, 0))
		{
			AfxMessageBox(IDS_FAILED_LAUNCH_HELP, MB_ICONINFORMATION);
			return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}
