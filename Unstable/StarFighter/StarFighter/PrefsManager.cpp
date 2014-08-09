#include "PrefsManager.h"

PrefsManager* PREFS = NULL;

PrefsManager::PrefsManager()
{
	Init();
	//TODO: read prefs
}

PrefsManager::~PrefsManager()
{
	//TODO
}


void PrefsManager::Init()
{
	m_gameLoopSampling = 18;	//18ms ~ 56 Hz
}

