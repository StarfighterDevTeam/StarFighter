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

#define	GHOST_ALPHA_VALUE						120

#define JOYSTICK_MIN_AXIS_VALUE					25

#define TEXTURE_SIZE_LIMITATION						8187

#define PIXEL_PERFECT_COLLISION							true
#define GRAZE_DISTANCE									30

#define DEFAULT_MUSIC_VOLUME						50
#define DEFAULT_SFX_VOLUME							100

//FLUX SPECIFIC
#define GRID_WIDTH									50
#define GRID_HEIGHT									50
#define TILE_SIZE									128
#define FLUX_TRANSFER_LIMITER_TIME					0.1f

//PLAYER
#define SHIP_MAX_FLUX								100
#define PLAYER_FLUX_DISPLAY_OFFSET_Y				4

//FLUXOR
#define FLUXOR_FLUX_VALUE							20
#define FLUXOR_FLUX_DISPLAY_OFFSET_Y				2

#define FLUXOR_WIDTH								64
#define FLUXOR_HEIGHT								48
#define FLUXOR_SPEED_MIN							100
#define FLUXOR_SPEED_MAX							500
#define FLUXOR_TURN_MIN_DELAY						0.5f
#define FLUXOR_TURN_MAX_DELAY						3.f
#define FLUXOR_MAX_POPULATION						100
#define FLUXOR_RESPAWN_MIN_TIME						5.f
#define FLUXOR_RESPAWN_MAX_TIME						60.f

//MODULE
#define MODULE_FLUX_DISPLAY_OFFSET_Y				- 24
#define MODULE_GLOW_MIN_RADIUS						10
#define MODULE_GLOW_RADIUS							30
#define MODULE_GLOW_ANIMATION_DURATION				0.5f
#define MODULE_A_FLUX_MAX							200
#define MODULE_B_FLUX_MAX							50
#define MODULE_C_FLUX_MAX							100
#define MODULE_O_FLUX_MAX							10

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED