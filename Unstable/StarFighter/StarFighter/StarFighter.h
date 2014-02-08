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

//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

#endif // STARFIGHTER_H_INCLUDED