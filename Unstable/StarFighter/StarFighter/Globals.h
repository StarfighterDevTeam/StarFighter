#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include "Logger.h"
#include <assert.h>

#define PLAYER_SAVE_FILE						"Saves/KnownScenes.txt"

//DISPLAY
#define	REF_WINDOW_RESOLUTION_X					1920
#define REF_WINDOW_RESOLUTION_Y					1080

#define	WINDOW_RESOLUTION_X						1600
#define WINDOW_RESOLUTION_Y						900

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define SHIP_START_X								990
#define SHIP_START_Y								REF_WINDOW_RESOLUTION_Y/2

//GAME RULES
#define GOAL_RESPAWN_COOLDOWN						2.f
#define USE_FRICTION_ON_DISCOBALL					false
#define COEFF_FRICTION_INCREASE_RATIO				0.005f
#define DISCOBALL_MIN_SPEED							20.f
#define SAFE_ZONE_X									200.f
#define SCORE_AGAIN_COOLDOWN						2.f

//STROBOSCOPIC EFFECT
#define ANGULARSPEED_FOR_STROBO_ACTIVATION			15.f
#define ANGULARSPEED_FOR_FULL_STROBO				30.f
#define STROBO_EFFECT_DURATION						0.2f
#define STROBO_EFFECT_TIME_BETWEEN_POSES			0.0005f

//BUMPERS DRAWING
#define BUMPER_WIDTH								4.f
#define BUMPER_OUTSIDE_GLOW_ALPHA					100

//SHIP MANEUVERABILITY
#define SHIP_ACCELERATION							100.f
#define SHIP_DECELERATION_COEF						5000.f
#define SHIP_MAX_SPEED								700.f
#define SHIP_MIN_SPEED								50.f
#define SHIP_STRAFFING_SPEED_MALUS					0.8f

//CLICK
#define SHIP_CLICK_INPUT_PRECISION					10.f
#define SHIP_CLICK_ANGLE_FOR_UTURN					30
#define SHIP_CLICK_UTURN_INPUT_VALUE				0.1f

//TACKLE
//#define SHIP_MIN_SPEED_FOR_TACKLE					200.f
#define SHIP_TACKLE_ACCELERATION_RATIO				0.4f
#define SHIP_MAX_TACKLE_SPEED_BONUS_RATIO			2.5f
#define SHIP_SPEED_PERCENTAGE_ON_HOLDING_TACKLE		0.7f
#define SHIP_TACKLE_MIN_LASTING_TIME				0.1f
#define SHIP_TACKLE_MAX_HOLD_TIME					0.3f
#define SHIP_TACKLE_HOLDDECELERATION_COFF			0.05f
#define SHIP_TACKLE_DECELERATION_COFF				0.5f
#define SHIP_MIN_SPEED_AFTER_TACKLE					100.f

//BRAWL
#define SHIP_BRAWL_DURATION							0.07f
#define SHIP_SPEED_ON_BRAWLING						1500.f

//DODGE
#define SHIP_MIN_SPEED_FOR_DODGE					1.f
#define SHIP_DODGE_DURATION							0.3f
#define SHIP_SPEED_BONUS_ON_DODGING					1.5f

//RELEASE
#define DISCOBALL_RELEASE_SPEED_RATIO				0.3f

//COOLDOWNS
#define CARRY_AGAIN_COOLDOWN						0.5f
#define TACKLE_AGAIN_COOLDOWN						0.5f
#define BRAWL_AGAIN_COOLDOWN						0.3f
#define DODGE_AGAIN_COOLDOWN						0.5f

//RECOVERY
#define RECOVERING_FROM_BRAWL_COOLDOWN				0.2f
#define RECOVERING_FROM_TACKLE_COOLDOWN				0.8f

//DISCOBALL CARRY
#define	DISCOBALL_GRAVITATION_DISTANCE				70.f
#define	CARRY_BASE_ANGULAR_SPEED					2*M_PI
#define	CARRY_MAX_ANGULAR_SPEED						8*M_PI
#define	CARRY_TIME_FOR_DECELERATION					0.5f
#define	CARRY_ANGULAR_DECELERATION					1.f
#define	CARRY_THROW_ACCELERATION_BONUS				6.f
#define	CARRY_CATCH_ACCELERATION_MALUS				4.f
#define THROW_BONUS_SPEED_TIMER						0.3f
#define SHIP_MAX_SPEED_MULTIPLIER_AFTER_THROW		20.f

//FEEDBACKS
#define	GHOST_ALPHA_VALUE							120
#define RESET_FEEDBACK_COOLDOWN						0.5f

//CONTROLS
#define JOYSTICK_MIN_AXIS_VALUE						25

//COLLISION
#define PIXEL_PERFECT_COLLISION						true

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED