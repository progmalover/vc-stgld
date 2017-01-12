#include "StdAfx.h"
#include "gpair.h"
#include "GlandaDoc.h"

namespace GPAIR
{
	BOOL ALWAYS_LIST_SOUND	= FALSE;
	BOOL ALWAYS_LIST_INST	= FALSE;

	GEFFECT_PAIR GetBeginEffectPair()	// 获得第一个特效
	{
		gldScene2 *pScene = _GetCurScene2();
		if (pScene)
		{
			int nRowCount = 0;
			if(ALWAYS_LIST_SOUND || pScene->m_soundList.size())
			{
				nRowCount += 1;
				if(pScene->m_soundExpanded)
				{
					nRowCount += pScene->m_soundList.size();
				}
			}
			for(GINSTANCE_LIST::reverse_iterator iInstance = pScene->m_instanceList.rbegin(); iInstance != _GetCurScene2()->m_instanceList.rend(); ++iInstance)
			{
				gldInstance* pInstance = *iInstance;
				if(ALWAYS_LIST_INST || pInstance->m_effectList.size())
				{
					nRowCount += 1;
					if(pInstance->m_expanded)
					{
						for(GEFFECT_LIST::iterator iEffect = pInstance->m_effectList.begin(); iEffect != pInstance->m_effectList.end(); ++iEffect)
						{
							return GEFFECT_PAIR(iInstance, iEffect, nRowCount);
						}
					}
				}
			}
		}

		// 返回EndPair
		return GEFFECT_PAIR();
	}

	GEFFECT_PAIR GetNextEffectPair(GEFFECT_PAIR& gPair)	// 获得下一个特效
	{
		int nRowCount = gPair.nIndex + 1;

		gldInstance* pInstance = *gPair.iterInstance;
		GINSTANCE_LIST::reverse_iterator iInstance = gPair.iterInstance;
		GEFFECT_LIST::iterator iEffect = gPair.iterEffect;
		for(++iEffect; iEffect != pInstance->m_effectList.end(); ++iEffect)
		{
			return GEFFECT_PAIR(iInstance, iEffect, nRowCount);
		}

		for(++iInstance; iInstance != _GetCurScene2()->m_instanceList.rend(); ++iInstance)
		{
			gldInstance* pInstance = *iInstance;
			if(ALWAYS_LIST_INST || pInstance->m_effectList.size())
			{
				nRowCount += 1;
				if(pInstance->m_expanded)
				{
					for(GEFFECT_LIST::iterator iEffect = pInstance->m_effectList.begin(); iEffect != pInstance->m_effectList.end(); ++iEffect)
					{
						return GEFFECT_PAIR(iInstance, iEffect, nRowCount);
					}
				}
			}
		}

		// 返回EndPair
		return GEFFECT_PAIR();
	}

	GINSTANCE_PAIR GetBeginInstPair()
	{
		gldScene2 *pScene = _GetCurScene2();
		if (pScene)
		{
			int nRowCount = 0;
			if(ALWAYS_LIST_SOUND || pScene->m_soundList.size())
			{
				nRowCount += 1;
				if(pScene->m_soundExpanded)
				{
					nRowCount += pScene->m_soundList.size();
				}
			}
			for(GINSTANCE_LIST::reverse_iterator iInstance = pScene->m_instanceList.rbegin(); iInstance != _GetCurScene2()->m_instanceList.rend(); ++iInstance)
			{
				gldInstance* pInstance = *iInstance;
				if(ALWAYS_LIST_INST || pInstance->m_effectList.size())
				{
					return GINSTANCE_PAIR(iInstance, nRowCount);
				}
			}
		}

		// 返回EndPair
		return GINSTANCE_PAIR();
	}

	GINSTANCE_PAIR GetNextInstPair(GINSTANCE_PAIR& gPair)
	{
		GINSTANCE_LIST::reverse_iterator iInstance = gPair.iterInstance;
		gldInstance* pInstance = *iInstance;
		int nRowCount = gPair.nIndex + 1;
		if(pInstance->m_expanded)
		{
			nRowCount += pInstance->m_effectList.size();
		}

		for(++iInstance; iInstance != _GetCurScene2()->m_instanceList.rend(); ++iInstance)
		{
			gldInstance* pInstance = *iInstance;
			if(ALWAYS_LIST_INST || pInstance->m_effectList.size())
			{
				return GINSTANCE_PAIR(iInstance, nRowCount);
			}
		}

		// 返回EndPair
		return GINSTANCE_PAIR();
	}

	GSOUND_PAIR GetBeginSoundPair()
	{
		gldScene2* pScene = _GetCurScene2();
		if(pScene)
		{
			int nRowCount = 0;
			nRowCount += 1;
			if(ALWAYS_LIST_SOUND || pScene->m_soundList.size())
			{
				if(pScene->m_soundExpanded)
				{
					for(GSCENESOUND_LIST::iterator iSound = pScene->m_soundList.begin(); iSound != pScene->m_soundList.end(); ++iSound)
					{
						return GSOUND_PAIR(iSound, nRowCount);
					}
				}
			}
		}
		return GSOUND_PAIR();
	}

	GSOUND_PAIR GetNextSoundPair(GSOUND_PAIR& gPair)
	{
		gldScene2* pScene = _GetCurScene2();
		if(pScene)
		{
			int nRowCount = gPair.nIndex;
			nRowCount += 1;
			GSCENESOUND_LIST::iterator iSound = gPair.iterSound;
			for(++iSound; iSound != pScene->m_soundList.end(); ++iSound)
			{
				return GSOUND_PAIR(iSound, nRowCount);
			}
		}
		return GSOUND_PAIR();
	}

	GSCAPT_PAIR GetBeginScaptPair()
	{
		gldScene2* pScene = _GetCurScene2();
		if(pScene)
		{
			int nRowCount = 0;
			if(ALWAYS_LIST_SOUND || pScene->m_soundList.size())
			{
				return GSCAPT_PAIR(nRowCount);
			}
		}

		return GSCAPT_PAIR();
	}

	GSCAPT_PAIR GetNextScaptPair(GSCAPT_PAIR& gPair)
	{
		return GSCAPT_PAIR();
	}


	gldInstance* InstanceFromPair(GEFFECT_PAIR& gPair)	// 获得Instance
	{
		return gPair.nIndex < 0 ? NULL : *gPair.iterInstance;
	}

	gldEffect* EffectFromPair(GEFFECT_PAIR& gPair)		// 获得特效
	{
		return gPair.nIndex < 0 ? NULL : *gPair.iterEffect;
	}

	gldInstance* InstanceFromPair(GINSTANCE_PAIR& gPair)
	{
		return gPair.nIndex < 0 ? NULL : *gPair.iterInstance;
	}

	gldSceneSound* SoundFromPair(GSOUND_PAIR& gPair)
	{
		return gPair.nIndex < 0 ? NULL : *gPair.iterSound;
	}

	GEFFECT_PAIR PairFromEffect(gldInstance* pInstance, gldEffect* pEffect)
	{
		for(GEFFECT_PAIR gPair = GetBeginEffectPair(); !IsEndPair(gPair); gPair = GetNextEffectPair(gPair))
		{
			if(*gPair.iterInstance==pInstance && *gPair.iterEffect==pEffect)
			{
				return gPair;
			}
		}
		return GEFFECT_PAIR();
	}

	GINSTANCE_PAIR PairFromInst(gldInstance* pInstance)
	{
		for(GINSTANCE_PAIR gPair = GetBeginInstPair(); !IsEndPair(gPair); gPair = GetNextInstPair(gPair))
		{
			if(*gPair.iterInstance==pInstance)
			{
				return gPair;
			}
		}
		return GINSTANCE_PAIR();
	}

	GSOUND_PAIR PairFromSound(gldSceneSound* pSound)
	{
		for(GSOUND_PAIR gPair = GetBeginSoundPair(); !IsEndPair(gPair); gPair = GetNextSoundPair(gPair))
		{
			if(*gPair.iterSound==pSound)
			{
				return gPair;
			}
		}
		return GSOUND_PAIR();
	}

	GSCAPT_PAIR PairFromScapt()
	{
		for(GSCAPT_PAIR gPair = GetBeginScaptPair(); !IsEndPair(gPair); gPair = GetNextScaptPair(gPair))
		{
			return gPair;
		}
		return GSCAPT_PAIR();
	}
};