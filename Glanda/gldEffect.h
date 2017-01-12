#pragma once

#include <string>
#include <list>

#include "..\CommonDef\CommonDef.h"

class gldEffect;

class gldEffectInfo
{
public:
	gldEffectInfo();
	~gldEffectInfo();

protected:
	BOOL m_init;

public:
	enum EffectType
	{
		typeEnter = 0, 
		typeEmphasize, 
		typeExit, 
		typeMotionPath1, 
		typeMotionPath2
	}m_type;
	
	BOOL Init(REFIID clsid);
	HBITMAP GetBitmap();	// CNewMenu call this one, because it may modify the object

	CLSID m_clsid;
	std::string m_name;
	std::string m_description;
	bool m_autoShowConfig;	
	CBitmap m_bitmap;		// CTimeline uses this cached one for performance.
	int m_iconIndex;

	UINT BrowseForProperties(HWND parent, int x, int y, const CString &caption, int nPages, const CLSID *pPages, int nActivate, LPUNKNOWN props);

protected:
	std::string m_resourceModule;
	UINT m_bitmapID;
};

class gldEffect
{
public:
	gldEffect();
	gldEffect(REFIID clsid);
	~gldEffect(void);

public:
	CLSID m_clsid;
	CComPtr<IGLD_Effect> m_pIEffect;
	CComPtr<IGLD_Parameters> m_pIParas;

	int m_startTime;
	int m_length;

	int m_startTimeTracking;
	int m_lengthTracking;

	gldEffectInfo *m_pEffectInfo;

	std::string GetName();	
	void ChangeTo(REFIID clsid, IGLD_Parameters *paras);

protected:
	std::string m_name;				// cached name. useful is the effect is 
									// not registered on another system

public:
	//void SetTime(int startTime, int length, BOOL bNotify);
	void CalcIdealLength(int nTotalCharacterKeys);

	gldEffect *Clone() const;
	void Copy(const gldEffect *pSrc);

	void GetToolTipText(CString &strText);

	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);
};

typedef std::list<gldEffect *> GEFFECT_LIST;


#include "Command.h"
class CCmdChangeEffectParameters : public TCommand
{
public:
	CCmdChangeEffectParameters(IGLD_Parameters *pTarget, IGLD_Parameters *pNew);
	~CCmdChangeEffectParameters();
	bool Execute();
	bool Unexecute();
private:
	CComPtr<IGLD_Parameters> m_pTarget;
	CComPtr<IGLD_Parameters> m_pOld;
	CComPtr<IGLD_Parameters> m_pNew;
};

inline bool IsPathEffect(gldEffect *pEffect)
{
	ASSERT(pEffect != NULL);

	if (pEffect->m_pEffectInfo != NULL)
	{
		return (pEffect->m_pEffectInfo->m_type == gldEffectInfo::typeMotionPath1 || 
			pEffect->m_pEffectInfo->m_type == gldEffectInfo::typeMotionPath2);
	}
	else
	{
		return false;
	}
};

