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

#define	SCENE_SIZE_X							1920
#define SCENE_SIZE_Y							1080

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define SHIP_START_X								990
#define SHIP_START_Y								540

//SHIP MANEUVERABILITY
#define SHIP_ACCELERATION							100.f
#define SHIP_DECELERATION_COEF						5000.f
#define SHIP_MAX_SPEED								500.f
#define SHIP_MIN_SPEED								50.f

//TACKLE
#define SHIP_MIN_SPEED_FOR_TACKLE					200.f
#define SHIP_TACKLE_ACCELERATION_RATIO				0.2f
#define SHIP_MAX_TACKLE_SPEED_BONUS_RATIO			2.5f
#define SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE		0.7f
#define SHIP_TACKLE_MIN_LASTING_TIME				0.f
#define SHIP_TACKLE_MAX_HOLD_TIME					0.2f
#define SHIP_TACKLE_HOLDDECELERATION_COFF			0.1f
#define SHIP_TACKLE_DECELERATION_COFF				0.5f
#define SHIP_MIN_SPEED_AFTER_TACKLE					100.f

//COOLDOWNS
#define CARRY_AGAIN_COOLDOWN						0.2f
#define TACKLE_AGAIN_COOLDOWN						0.5f

//DISCOBALL
#define	DISCOBALL_GRAVITATION_DISTANCE				70.f
#define	DISCOBALL_BASE_ANGULAR_SPEED				M_PI
#define	CARRY_TIME_FOR_ACCELERATION					0.4f
#define	CARRY_MAX_STOCKS_OF_ACCELERATION			10
#define	CARRY_ANGULAR_ACCELERATION					2.f
#define	CARRY_THROW_ACCELERATION_BONUS				6.f
#define	CARRY_CATCH_ACCELERATION_MALUS				8.f
#define THROW_BONUS_SPEED_TIMER						0.3f
#define SHIP_MAX_SPEED_MULTIPLIER_AFTER_THROW		20.f

//FEEDBACKS
#define	GHOST_ALPHA_VALUE							120
#define RESET_FEEDBACK_COOLDOWN						0.5f

#define JOYSTICK_MIN_AXIS_VALUE						25

#define PIXEL_PERFECT_COLLISION						true

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED