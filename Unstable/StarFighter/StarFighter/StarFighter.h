#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <SFML/Audio.hpp>
#include "Globals.h"
#include "PrefsManager.h"
#include "TextureLoader.h"
#include "Assets.h"
#include "GameObject.h"
#include "Game.h"
#include "Logger.h"
#include "InGameState.h"
#include "GameManager.h"
#include "Player.h"
#include "Ship.h"

#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() Logger::Stop();

//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

//Initialize Logger Singleton
Logger Logger::instance;
Logger::Logger() : active(false) {}

#endif // STARFIGHTER_H_INCLUDED