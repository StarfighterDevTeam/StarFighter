#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <SFML/Audio.hpp>
#include "Globals.h"
#include "PrefsManager.h"
#include "TextureLoader.h"
#include "Game.h"
#include "InGameState.h"
#include "GameManager.h"
#include "Player.h"


//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

#endif // STARFIGHTER_H_INCLUDED