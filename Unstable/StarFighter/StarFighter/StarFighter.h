#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <SFML/Audio.hpp>
#include "Globals.h"
#include "Ship.h"
#include "Background.h"
#include "Weapon.h"
#include "TextureLoader.h"
#include "Assets.h"
#include "Enemy.h"
#include "Ammo.h"
#include "FX.h"
#include "Loot.h"
#include "Scene.h"
#include "Logger.h"
#include "MovePattern.h"

#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() Logger::Stop();

//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

//Initialize Logger Singleton
Logger Logger::instance;
Logger::Logger() : active(false) {}

#endif // STARFIGHTER_H_INCLUDED