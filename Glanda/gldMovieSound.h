#pragma once
#include <string>
class gldSoundInstance;

class gldMovieSound
{
public:
	gldMovieSound();
	virtual ~gldMovieSound(void);

	std::string m_name;
	std::string m_pathname;
	gldSoundInstance* m_sound;
	std::string m_startScene;

	HRESULT WriteToStream(IStream *pStm, void *pUnk);
	HRESULT ReadFromStream(IStream *pStm, void *pUnk);
};
