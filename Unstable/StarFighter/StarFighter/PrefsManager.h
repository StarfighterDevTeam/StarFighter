#ifndef PREFSMANAGER_H_INCLUDED
#define PREFSMANAGER_H_INCLUDED

#include <string.h>

class PrefsManager
{
public:
	PrefsManager();
	~PrefsManager();

	unsigned int m_gameRefreshRateHz;

protected:
	void Init();

};

extern PrefsManager*	PREFS;	// global and accessable from anywhere 

#endif //PREFSMANAGER_H_INCLUDED