#pragma once

#include <vector>

class gldEffectInfo;
typedef std::vector<gldEffectInfo *> GEFFECTINFO_LIST;

class gldEffectManager
{
public:
	gldEffectManager(void);
	~gldEffectManager(void);

	DECLARE_SINGLETON(gldEffectManager)

	enum {ICON_DEFAULT_EFFECT = 0
		, ICON_CAT_ENTRANCE = 1
		, ICON_CAT_EMPHASIS = 2
		, ICON_CAT_EXIT = 3
		, ICON_CAT_PATH = 4};

private:
	GEFFECTINFO_LIST m_effectInfoList;
	CImageList m_ilEffect;
	
public:
	static LPCTSTR EFFECT_REGISTRY_KEY;
	static const COLORREF ICON_TRANSPARENT_COLOR;
	enum {CX_ICON = 16};
	enum {CY_ICON = 16};
	
private:
	void LoadFromRigistry();

public:
	GEFFECTINFO_LIST &GetEffectInfoList() {return m_effectInfoList;}
	CImageList *GetImageList();
	gldEffectInfo *FindEffect(REFIID clsid);
	void GetImageList(CImageList &il);

};
