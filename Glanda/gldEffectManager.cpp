#include "StdAfx.h"
#include ".\gldeffectmanager.h"
#include "gldEffect.h"
#include <algorithm>
#include "Resource.h"

bool CompareEffects(gldEffectInfo *pInfo1, gldEffectInfo *pInfo2)
{
	if (pInfo1->m_type != pInfo2->m_type)
		return pInfo1->m_type < pInfo2->m_type;
	else
		return _tcsicmp(pInfo1->m_name.c_str(), pInfo2->m_name.c_str()) <= 0;
}

IMPLEMENT_SINGLETON(gldEffectManager)

LPCTSTR gldEffectManager::EFFECT_REGISTRY_KEY = "CLSID\\{D35026DC-C7E9-4193-B680-7E18FCDDE09B}";
const COLORREF gldEffectManager::ICON_TRANSPARENT_COLOR = RGB(255, 0, 255);

gldEffectManager::gldEffectManager(void)
{
	LoadFromRigistry();
}

gldEffectManager::~gldEffectManager(void)
{
	for (GEFFECTINFO_LIST::iterator it = m_effectInfoList.begin(); it != m_effectInfoList.end(); it++)
	{
		gldEffectInfo *pEffectInfo = *it;
		delete pEffectInfo;
	}
	m_effectInfoList.clear();
}

gldEffectInfo *gldEffectManager::FindEffect(REFIID clsid)
{
	for (GEFFECTINFO_LIST::iterator it = m_effectInfoList.begin(); it != m_effectInfoList.end(); it++)
	{
		gldEffectInfo *pEffectInfo = *it;
		if (::IsEqualGUID(pEffectInfo->m_clsid, clsid))
			return pEffectInfo;
	}
	return NULL;
}

void gldEffectManager::LoadFromRigistry()
{
	HKEY hCat= NULL;
	DWORD dwRet = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, EFFECT_REGISTRY_KEY, 0, KEY_READ, &hCat);
	if (dwRet != ERROR_SUCCESS)
	{
		::SetLastError(dwRet);
		::ReportLastError("Failed to load effect list: %s");
	}
	else
	{
		DWORD dwIndex = 0;
		TCHAR szKey[255]; // 255 is the limit of Windows 9x
		while (::RegEnumKey(hCat, dwIndex, szKey, 255) == ERROR_SUCCESS)
		{
			dwIndex++;

			CComBSTR bstrKey(szKey);
			CLSID clsid;
			if (SUCCEEDED(CLSIDFromString(bstrKey, &clsid)))
			{
				gldEffectInfo *pEffectInfo = new gldEffectInfo();
				if (!pEffectInfo->Init(clsid))
				{
#ifdef _DEBUG
					BSTR bstrProgID = NULL;
					if (SUCCEEDED(ProgIDFromCLSID(clsid, &bstrProgID)))
					{
						CString strProgID(bstrProgID);
						::CoTaskMemFree(bstrProgID);

						AfxMessageBoxEx(MB_ICONWARNING, "Failed to initialize effect %s.", (LPCTSTR)strProgID);
					}
#endif
					delete pEffectInfo;
				}
				else
				{
					// Get effect type
					HKEY hSubKey = NULL;
					CString strSubKey;
					strSubKey.Format("%s\\%s", EFFECT_REGISTRY_KEY, szKey); 
					if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, strSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
					{
						DWORD dwEffectType;
						DWORD dwType;
						DWORD dwSize = sizeof(DWORD);
						if (::RegQueryValueEx(hSubKey, NULL, NULL, &dwType, (LPBYTE)&dwEffectType, &dwSize) == ERROR_SUCCESS)
							pEffectInfo->m_type = (gldEffectInfo::EffectType)dwEffectType;

						::RegCloseKey(hSubKey);
					}
					// Add always, even if the type can not be retrieved.
					m_effectInfoList.push_back(pEffectInfo);
				}
			}
		}

		::RegCloseKey(hCat);

		if (m_effectInfoList.size() >= 2)
		{
			std::sort(m_effectInfoList.begin(), m_effectInfoList.end(), CompareEffects);
		}
	}

	GetImageList(m_ilEffect);
}

CImageList *gldEffectManager::GetImageList()
{
	if (m_ilEffect.m_hImageList == NULL)
	{
		GetImageList(m_ilEffect);
	}
	return &m_ilEffect;
}

void gldEffectManager::GetImageList(CImageList &il)
{
	ASSERT(il.m_hImageList == NULL);
	VERIFY(il.Create(CX_ICON, CY_ICON, ILC_MASK | ILC_COLOR32, 0, 1));
	CBitmap iconItem;
	VERIFY(iconItem.LoadBitmap(IDB_EFFECT_DEFAULT));
	VERIFY(il.Add(&iconItem, RGB(255, 0, 255)) == 0);
	CBitmap iconCats;
	iconCats.LoadBitmap(IDB_EFFECT_CATS);
	VERIFY(il.Add(&iconCats, RGB(255, 0, 255)) == 1);
	for (GEFFECTINFO_LIST::iterator ite = m_effectInfoList.begin();
		ite != m_effectInfoList.end(); ++ite)
	{
		gldEffectInfo *pEffectInfo = *ite;
		int iconIndex = ICON_DEFAULT_EFFECT;
		HBITMAP icon = pEffectInfo->GetBitmap();
		if (icon != NULL)
		{
			CBitmap bmp;
			VERIFY(bmp.Attach(icon));
			BITMAP bi;
			memset(&bi, 0, sizeof(bi));
			if (bmp.GetBitmap(&bi) != 0 && bi.bmWidth == CX_ICON && bi.bmHeight == CY_ICON)
			{
				iconIndex = m_ilEffect.Add(&bmp, ICON_TRANSPARENT_COLOR);
			}
		}
		pEffectInfo->m_iconIndex = iconIndex;
	}
}