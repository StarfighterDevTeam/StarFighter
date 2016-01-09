#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include "Logger.h"
#include <assert.h>

#define PLAYER_SAVE_FILE						"Saves/KnownScenes.txt"

#define	REF_WINDOW_RESOLUTION_X					1920
#define REF_WINDOW_RESOLUTION_Y					1080

#define	WINDOW_RESOLUTION_X						1600
#define WINDOW_RESOLUTION_Y						900

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define	GHOST_ALPHA_VALUE						150

#define JOYSTICK_MIN_AXIS_VALUE					25

#define TEXTURE_SIZE_LIMITATION						8187

#define PIXEL_PERFECT_COLLISION						false
#define GRAZE_DISTANCE								30

#define DEFAULT_MUSIC_VOLUME						50
#define DEFAULT_SFX_VOLUME							100

//FLUX SPECIFIC
#define GRID_WIDTH									15
#define GRID_HEIGHT									15
#define TILE_SIZE									128
#define DEFAULT_TILE_START							5-1
#define DEFAULT_TILE_GENERATOR						4-1
#define CENTRAL_FLUXOR_ZONE_TILE_OFFSET				5
#define GRID_CELLS_FOR_MODULE_LINK_ACTIVATION		1
#define USE_UNGUIDED_FLUXORS_COST_MULTIPLIER		0.1f
#define USE_SPLIT_SCREEN							true

//HUD
#define FLUX_GAUGE_HEIGHT							10
#define FLUX_GAUGE_WIDTH							150

#define FLUX_GAUGE_COLOR_R							150
#define FLUX_GAUGE_COLOR_G							150
#define FLUX_GAUGE_COLOR_B							255
#define FLUX_GAUGE_COLOR_A							150
#define FLUX_GAUGE_THICNKESS						2
#define FLUX_GAUGE_OUTLINE_COLOR_A					255

#define BUILD_GAUGE_HEIGHT							10
#define BUILD_GAUGE_WIDTH							150
#define BUILD_GAUGE_COLOR_R							150
#define BUILD_GAUGE_COLOR_G							255
#define BUILD_GAUGE_COLOR_B							150
#define BUILD_GAUGE_COLOR_A							150
#define FLUX_GAUGE_THICNKESS						2
#define BUILD_GAUGE_OUTLINE_COLOR_A					255

//PLAYER
#define SHIP_MAX_FLUX								100
#define PLAYER_FLUX_DISPLAY_OFFSET_Y				4
#define SHIP_MAX_FLUX_BONUS_PER_LEVEL				100
#define SHIP_MAX_SPEED_BONUS_PER_LEVEL				100
#define SHIP_MAX_UPGRADE_LEVEL						9

#define SHIP_START_X                990
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		5000.0f
#define SHIP_MAX_SPEED				400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

//FLUXOR
#define FLUXOR_FLUX_VALUE							10
#define	GREEN_FLUXOR_VALUE							5
#define FLUXOR_FLUX_DISPLAY_OFFSET_Y				2

#define FLUXOR_WIDTH								64
#define FLUXOR_HEIGHT								48
#define FLUXOR_SPEED_MIN							100
#define FLUXOR_SPEED_MAX							500
#define FLUXOR_GUIDED_BASE_SPEED					100
#define FLUXOR_GUIDED_MAX_SPEED						1000
#define FLUXOR_BULLET_SPEED							500
#define FLUXOR_TURN_MIN_DELAY						0.05f
#define FLUXOR_TURN_MAX_DELAY						3.f
#define FLUXOR_MAX_POPULATION						20
#define FLUXOR_RESPAWN_MIN_TIME						5.f
#define FLUXOR_RESPAWN_MAX_TIME						60.f
#define FLUXOR_WASTE_DELAY							0.5f
#define FLUXOR_ATTACK_DELAY							0.5f
#define FLUXOR_WASTE_VALUE							1

//MODULE
#define MODULE_FLUX_DISPLAY_OFFSET_Y				- 24
#define MODULE_GLOW_MIN_RADIUS						1
#define MODULE_GLOW_RADIUS							12
#define MODULE_GLOW_ANIMATION_DURATION				0.5f
#define MODULE_TRANSFER_DELAY						0.1f
#define MODULE_FLUX_CONSTRUCTION_PER_SECOND			5

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED