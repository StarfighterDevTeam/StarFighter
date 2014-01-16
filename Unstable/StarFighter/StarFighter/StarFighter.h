#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

#include <stdio.h>
#include <cmath>
#include "Globals.h"
#include "Ship.h"
#include "Background.h"
#include "Weapon.h"
#include "Ammo.h"
#include "TextureLoader.h"

//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

#endif // STARFIGHTER_H_INCLUDED