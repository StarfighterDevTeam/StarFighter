#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Game.h"
#include "TextUtils.h"
#include "Logger.h"


#define	WINDOW_RESOLUTION_X						800
#define WINDOW_RESOLUTION_Y						600

#define	SCENE_SIZE_X							800
#define SCENE_SIZE_Y							600

#define LOOP_SAMPLING_MS						18	//18ms ~ 56 Hz

#define SHIP_FILE "Assets/Scripts/Ship.csv"
#define ENEMY_FILE	"Assets/Scripts/Enemies.csv"
#define WEAPON_FILE	"Assets/Scripts/Weapons.csv"
#define AMMO_FILE "Assets/Scripts/Ammo.csv"
#define ENEMYPOOL_FILE "Assets/Scripts/EnemyPool.csv"
#define FX_FILE "Assets/Scripts/FX.csv"

#endif // GLOBALS_H_INCLUDED