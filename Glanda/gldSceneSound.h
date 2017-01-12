#pragma once

#include <string>
#include <list>

class gldSoundInstance;

class gldSceneSound
{
public:
	gldSceneSound(void);
	~gldSceneSound(void);

public:
	std::string m_name;
	int m_startTime;
	int m_length;

	int m_startTimeTracking;
	int m_lengthTracking;

	gldSoundInstance *m_sound;

	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);

	void GetToolTipText(CString& strText);
};

typedef std::list<gldSceneSound *> GSCENESOUND_LIST;
