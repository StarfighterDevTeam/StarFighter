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
	m_gameRefreshRateHz = 60; //18ms ~ 56 Hz
}

