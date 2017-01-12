#pragma once

#include "gldSceneSound.h"
#include "gldScene2.h"
#include "gldInstance.h"


namespace GPAIR
{
	extern BOOL ALWAYS_LIST_SOUND;
	extern BOOL ALWAYS_LIST_INST;

	// Sound Caption
	struct GSCAPT_PAIR
	{
		int nIndex;

		GSCAPT_PAIR(int index) : nIndex(index)
		{
		}
		GSCAPT_PAIR() : nIndex(-1)
		{
		}
	};

	// Sound
	struct GSOUND_PAIR
	{
		GSCENESOUND_LIST::iterator iterSound;
		int nIndex;

		GSOUND_PAIR(GSCENESOUND_LIST::iterator iSound, int index) : iterSound(iSound), nIndex(index)
		{
		}
		GSOUND_PAIR() : nIndex(-1)
		{
		}
	};

	// Instance
	struct GINSTANCE_PAIR
	{
		GINSTANCE_LIST::reverse_iterator iterInstance;
		int nIndex;//��Ȼ����ͬλ������������㣬���nIndex ������ʲô���� ��

		GINSTANCE_PAIR(GINSTANCE_LIST::reverse_iterator iInstance, int index) : iterInstance(iInstance), nIndex(index)
		{
		}
		GINSTANCE_PAIR() : nIndex(-1)
		{
		}
	};

	// Effect
	struct GEFFECT_PAIR	// ��¼Instance����������Ч������
	{
		GINSTANCE_LIST::reverse_iterator iterInstance;
		GEFFECT_LIST::iterator iterEffect;
		int nIndex;

		GEFFECT_PAIR(GINSTANCE_LIST::reverse_iterator iInstance, GEFFECT_LIST::iterator iEffect, int index)
			: iterInstance(iInstance)
			, iterEffect(iEffect)
			, nIndex(index)
		{
		}
		GEFFECT_PAIR()
			: nIndex(-1)
		{
		}
	};


	template <class TA, class TB>
	BOOL IsSamePair(TA& gPairA, TB& gPairB)
	{
		return gPairA.nIndex == gPairB.nIndex;
	}

	template <class T>
	inline BOOL IsEndPair(T& gPair)		// �Ƿ�Ϊ��ֹ������
	{
		return gPair.nIndex<0;
	}

	template <class T>
	T GetEndPair()
	{
		return T();
	}

	template <class T>
	int IndexFromPair(T& gPair)
	{
		return gPair.nIndex;
	}

	GEFFECT_PAIR GetBeginEffectPair();						// �����ʼ��Ч������
	GEFFECT_PAIR GetNextEffectPair(GEFFECT_PAIR& gPair);	// �����һ����Ч������(gPair����Ϊ��ֹ������)
	GSOUND_PAIR GetBeginSoundPair();
	GSOUND_PAIR GetNextSoundPair(GSOUND_PAIR& gPair);
	GINSTANCE_PAIR GetBeginInstPair();
	GINSTANCE_PAIR GetNextInstPair(GINSTANCE_PAIR& gPair);
	GSCAPT_PAIR GetBeginScaptPair();
	GSCAPT_PAIR GetNextScaptPair(GSCAPT_PAIR& gPair);


	gldInstance* InstanceFromPair(GEFFECT_PAIR& gPair);		// ���Instance
	gldEffect* EffectFromPair(GEFFECT_PAIR& gPair);			// �����Ч
	gldInstance* InstanceFromPair(GINSTANCE_PAIR& gPair);	// ���Instance
	gldSceneSound* SoundFromPair(GSOUND_PAIR& gPair);		// ���Sound
	GEFFECT_PAIR PairFromEffect(gldInstance* pInstance, gldEffect* pEffect);	// �����Ч�ĵ�����
	GINSTANCE_PAIR PairFromInst(gldInstance* pInstance);
	GSOUND_PAIR PairFromSound(gldSceneSound* pSound);
	GSCAPT_PAIR PairFromScapt();

};