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

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define ENDSCENE_TRANSITION_SPEED_UP			600
#define ENDSCENE_TRANSITION_SPEED_DOWN			400	

#define HUB_EXIT_X_MIN_RATIO					0.1875f
#define HUB_EXIT_X_MAX_RATIO					0.8125f
#define HUB_EXIT_Y_MIN_RATIO					0.25f
#define HUB_EXIT_Y_MAX_RATIO					0.75f
#define HUB_EXIT_TIMER							1.f

#define SHIP_FILE "Assets/Scripts/Ship.csv"
#define ENEMY_FILE	"Assets/Scripts/Enemies.csv"
#define WEAPON_FILE	"Assets/Scripts/Weapons.csv"
#define AMMO_FILE "Assets/Scripts/Ammo.csv"
#define ENEMYPOOL_FILE "Assets/Scripts/EnemyPool.csv"
#define FX_FILE "Assets/Scripts/FX.csv"

#endif // GLOBALS_H_INCLUDED