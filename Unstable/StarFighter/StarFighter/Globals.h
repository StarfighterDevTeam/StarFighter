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
#define USE_FRICTION_ON_DISCOBALL					true
#define USE_ORTHOGONAL_THROW						true
#define COEFF_FRICTION_INCREASE_RATIO				0.005f
#define DISCOBALL_MIN_SPEED							20.f
#define DEFENDER_SAFE_ZONE_X						300.f
#define GOAL_SIZE									250.f
#define GOAL_SAFE_ZONE_X							100.f
#define SCORE_AGAIN_COOLDOWN						2.f
#define SCORE_DISPLAY_OFFSET						50
#define DELAY_AFTER_GOAL							2.f

//STROBOSCOPIC EFFECT
#define ANGULARSPEED_FOR_STROBO_ACTIVATION			10.f
#define STROBO_EFFECT_DURATION						0.01f
#define STROBO_EFFECT_TIME_BETWEEN_POSES			0.0005f
#define TACKLE_STROBO_EFFECT_DURATION				0.2f

//BUMPERS DRAWING
#define BUMPER_WIDTH								4.f
#define BUMPER_OUTSIDE_GLOW_ALPHA					100

//SHIP MANEUVERABILITY
#define SHIP_ACCELERATION							100.f
#define SHIP_DECELERATION_COEF						5000.f
#define SHIP_MAX_SPEED								600.f
#define SHIP_MIN_SPEED								50.f
#define SHIP_STRAFFING_SPEED_MALUS					0.8f

//CLICK
#define SHIP_CLICK_INPUT_PRECISION					10.f
#define SHIP_CLICK_ANGLE_FOR_UTURN					30
#define SHIP_CLICK_UTURN_INPUT_VALUE				0.1f

//TACKLE
#define SHIP_TACKLE_ACCELERATION					500.f
#define SHIP_TACKLE_MAX_SPEED						2000.f
#define SHIP_TACKLE_DECELERATION_WHILE_HOLDING		50.f
#define SHIP_TACKLE_MAX_HOLD_TIME					0.2f
#define SHIP_TACKLE_DECELERATION					100.f
#define SHIP_MIN_SPEED_AFTER_TACKLE					100.f

//RELEASE
#define DISCOBALL_RELEASE_SPEED_RATIO				0.7f

//COOLDOWNS
#define CARRY_AGAIN_COOLDOWN						0.5f
#define TACKLE_AGAIN_COOLDOWN						2.f

//RECOVERY
#define RECOVERING_FROM_TACKLE_COOLDOWN				0.8f

//DISCOBALL CARRY
#define	DISCOBALL_GRAVITATION_DISTANCE				70.f
#define	CARRY_BASE_ANGULAR_SPEED					2*M_PI
#define	CARRY_MAX_ANGULAR_SPEED						6*M_PI
#define	CARRY_TIME_FOR_DECELERATION					0.5f
#define	CARRY_ANGULAR_DECELERATION					1.f
#define	CARRY_THROW_ACCELERATION_BONUS				8.f
#define	CARRY_CATCH_ACCELERATION_MALUS				2.f
#define THROW_BONUS_SPEED_TIMER						0.3f
#define SHIP_MAX_SPEED_MULTIPLIER_AFTER_THROW		20.f

//FEEDBACKS
#define	GHOST_ALPHA_VALUE							120
#define RESET_FEEDBACK_COOLDOWN						0.5f

//CONTROLS
#define JOYSTICK_MIN_AXIS_VALUE						25

//COLLISION
#define PIXEL_PERFECT_COLLISION						true

//IA
#define IA_REACTION_TIME_EASY								0.5f
#define IA_REACTION_TIME_MEDIUM								0.2f
#define IA_REACTION_TIME_HARD								0.02f
#define IA_PRECISION_EASY									10
#define IA_PRECISION_MEDIUM									5
#define IA_PRECISION_HARD									3
#define IA_DISTANCE_FOR_UNMARKED							500
#define IA_DISTANCE_FOR_UNGUARDED							500
#define IA_DISTANCE_FOR_UNCONTESTED							750
#define IA_DISCOBALL_ANGLERAD_VARIATION_IS_SMALL			0.6f
#define IA_DISTANCE_FOR_SAFE_TACKLE							450

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED