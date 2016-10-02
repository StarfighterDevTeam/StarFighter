#ifndef STARENGINE_H_INCLUDED
#define STARENGINE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <SFML/Audio.hpp>
#include "Globals.h"
#include "TextureLoader.h"
#include "GameObject.h"
#include "SEGame.h"
#include "Logger.h"
#include "SEInGameState.h"
#include "SEShip.h"

#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() Logger::Stop();

//Handling various window resolutions
enum WindowResolutions
{
	RESOLUTION_1600x900,
	RESOLUTION_1920x1080_FullScreen,
	RESOLUTION_1280x720,
	RESOLUTION_1920x1080,
	NBVAL_RESOLUTIONS,
};

class StarEngine
{
private:
	sf::RenderWindow*	m_renderWindow;
	SEInGameState*		m_inGameState;
	unsigned int		m_frameRateLimit;
	std::string			m_iconName;
	std::string			m_title;

public:
	bool Init(const char* title, const char* iconName, WindowResolutions defaultResolution=RESOLUTION_1600x900);
	void Run();
	void Shut();
};

#endif // STARENGINE_H_INCLUDED
