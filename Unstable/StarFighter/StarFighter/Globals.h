#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include "Logger.h"

#define	REF_WINDOW_RESOLUTION_X					800
#define REF_WINDOW_RESOLUTION_Y					600

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

#define HAZARD_BREAK_RATIO						0.15f
#define HAZARD_BREAK_MULTIPLIER					0.3f
#define HAZARD_BAR_SIZE_X						20
#define HAZARD_BAR_SIZE_Y						500


#define SHIP_FILE "Assets/Scripts/Ship.csv"
#define ENEMY_FILE	"Assets/Scripts/Enemies.csv"
#define WEAPON_FILE	"Assets/Scripts/Weapons.csv"
#define AMMO_FILE "Assets/Scripts/Ammo.csv"
#define ENEMYPOOL_FILE "Assets/Scripts/EnemyPool.csv"
#define FX_FILE "Assets/Scripts/FX.csv"
#define EQUIPMENT_FILE "Assets/Scripts/Equipment.csv"
#define BOT_FILE "Assets/Scripts/Bot.csv"

#define GRAZE_DISTANCE									30
#define SHIP_BRAKING_SPEED									180

//DROP SYSTEM VARIABLES

#define EQUIPMENT_MIN_MAXSPEED_VALUE					100
#define EQUIPMENT_MAXSPEED_LN_MULTIPLIER_BASE			4
#define EQUIPMENT_MAXSPEED_LN_MULTIPLIER_X				0.01
#define EQUIPMENT_MAXSPEED_LN_MULTIPLIER_BONUS			2
#define EQUIPMENT_MIN_ACCELLERATION_VALUE				100
#define EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_BASE		4
#define EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_X			0.01
#define EQUIPMENT_ACCELLERATION_LN_MULTIPLIER_BONUS		2
#define EQUIPMENT_MIN_DECCELLERATION_VALUE				100
#define EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_BASE		5
#define EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_X		0.01
#define EQUIPMENT_DECCELLERATION_LN_MULTIPLIER_BONUS	2.5
#define EQUIPMENT_MIN_ARMOR_VALUE						100
#define EQUIPMENT_ARMOR_LN_MULTIPLIER_BASE				5
#define EQUIPMENT_ARMOR_LN_MULTIPLIER_X					0.01
#define EQUIPMENT_ARMOR_LN_MULTIPLIER_BONUS				2.5
#define EQUIPMENT_MIN_SHIELD_VALUE						100
#define EQUIPMENT_SHIELD_LN_MULTIPLIER_BASE				5
#define EQUIPMENT_SHIELD_LN_MULTIPLIER_X				0.01
#define EQUIPMENT_SHIELD_LN_MULTIPLIER_BONUS			2.5
#define EQUIPMENT_MIN_SHIELD_REGEN_VALUE				100
#define EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_BASE		5
#define EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_X			0.01
#define EQUIPMENT_SHIELD_REGEN_LN_MULTIPLIER_BONUS		2.5

#define WEAPON_MIN_DAMAGE_VALUE							50
#define WEAPON_DAMAGE_LN_MULTIPLIER_BASE				5
#define WEAPON_DAMAGE_LN_MULTIPLIER_X					0.01
#define WEAPON_DAMAGE_LN_MULTIPLIER_BONUS				2.5
#define WEAPON_MIN_VSPEED_VALUE							300
#define WEAPON_MIN_RATE_OF_FIRE_VALUE					0.5f

#define NUMBER_OF_BEASTSCALE_THRESHOLD					5
#define NB_MAX_PROPERTIES_FOR_NON_EPIC_EQUIPMENT		3

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);

#endif // GLOBALS_H_INCLUDED