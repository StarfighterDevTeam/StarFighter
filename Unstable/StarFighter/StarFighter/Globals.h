#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include "Logger.h"
#include <assert.h>

#define PLAYER_SAVE_FILE						"Saves/KnownScenes.txt"
#define ITEMS_SAVE_FILE							"Saves/ItemsStats.txt"
#define MONEY_SAVE_FILE							"Saves/Money.txt"

#define	REF_WINDOW_RESOLUTION_X					1920
#define REF_WINDOW_RESOLUTION_Y					1080

#define	WINDOW_RESOLUTION_X						1600
#define WINDOW_RESOLUTION_Y						900

#define	SCENE_SIZE_X							1080
#define SCENE_SIZE_Y							1080

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define STARTING_MONEY							50

#define DEFAULT_MUSIC_VOLUME					50
#define DEFAULT_SFX_VOLUME						50
#define MUSIC_FADE_IN_TIME						2.f
#define MUSIC_FADE_OUT_TIME						2.f

#define TIME_FOR_RECALL_TO_HUB					3
#define MIN_TIME_FOR_RECALL_TO_HUB				0.2f
#define TIME_BEFORE_BOSS_SPAWN					2
#define ENDSCENE_TRANSITION_SPEED_UP			1000
#define ENDSCENE_TRANSITION_SPEED_DOWN			500
#define HUB_EXIT_X_MIN_RATIO					0.1875f
#define HUB_EXIT_X_MAX_RATIO					0.8125f
#define HUB_EXIT_Y_MIN_RATIO					0.25f
#define HUB_EXIT_Y_MAX_RATIO					0.75f
#define HUB_EXIT_TIMER							1.f
#define HUB_LINK_NAME_OFFSET_RATIO				0.3f

#define	GHOST_ALPHA_VALUE						120
#define DAMAGE_FEEDBACK_TIME					0.04f
#define ENEMY_HEALTH_FEEDBACK_TIME				2.f

#define JOYSTICK_MIN_AXIS_VALUE					25

#define HUD_SCORES_SPACING							20
#define HUD_LEFT_MARGIN								20
#define HUD_CURSOR_SPEED							700
#define HUD_HOLD_TIME_BEFORE_REMOVE_ITEM			1.0f

#define SHIP_FILE "Assets/Scripts/Generated/Ship.csv"
#define ENEMY_FILE	"Assets/Scripts/Generated/Enemies.csv"
#define WEAPON_FILE	"Assets/Scripts/Generated/Weapons.csv"
#define AMMO_FILE "Assets/Scripts/Generated/Ammo.csv"
#define ENEMYPOOL_FILE "Assets/Scripts/Generated/EnemyPool.csv"
#define FX_FILE "Assets/Scripts/Generated/FX.csv"
#define EQUIPMENT_FILE "Assets/Scripts/Generated/Equipment.csv"
#define BOT_FILE "Assets/Scripts/Generated/Bot.csv"
#define SCENES_FILE "Assets/Scripts/Generated/Scenes.csv"
#define PHASES_FILE	"Assets/Scripts/Generated/EnemyPhases.csv"
#define DIALOGS_FILE "Assets/Scripts/Generated/Dialogs.csv"

#define PIXEL_PERFECT_COLLISION							true
#define GRAZE_DISTANCE									50
#define SHIP_BRAKING_MALUS_SPEED						0.50f
#define END_OF_DIALOGS_DELAY							1.5f

//HUD
#define ARMOR_BAR_SIZE_X								320
#define ARMOR_BAR_SIZE_Y								20
#define SHIELD_BAR_SIZE_Y								20
#define FUEL_BAR_SIZE_Y									14

#define XP_BAR_SIZE_X									320
#define XP_BAR_SIZE_Y									16

//DIALOGS
#define DIALOG_PANEL_WIDTH						800
#define DIALOG_PANEL_HEIGHT						150
#define DIALOG_PANEL_OFFSET_X					50
#define DIALOG_PANEL_OFFSET_Y					250
#define DIALOG_PANEL_PORTRAIT_SIZE_X			150
#define DIALOG_PANEL_PORTRAIT_SIZE_Y			150
#define DIALOG_PANEL_FADE_IN_TIME				1.5f
#define DIALOG_PANEL_FADE_OUT_TIME				0.5f
#define DIALOG_PANEL_ARROW_OFFSET_X				20

//SHOP
#define ITEM_STATS_SHOP_OFFSET_X						16
#define ITEM_STATS_SHOP_OFFSET_Y						16
#define MONEY_COST_OF_LOOT_CREDITS						10
#define NUMBER_OF_OBJECTS_GENERATED_IN_SHOP				9
#define NUMBER_OF_RARE_OBJECTS_GENERATED_IN_SHOP		1

//ENEMY LIFE BARS
#define ENEMY_HP_BAR_CONTAINER_SIZE_X					90
#define ENEMY_HP_BAR_CONTAINER_SIZE_Y					8
#define ENEMY_HP_BAR_OFFSET_Y							4
#define ENEMY_SHIELD_BAR_OFFSET_Y						4
#define ENEMY_LEVEL_DISPLAY_OFFSET_X					12
#define ENEMY_LEVEL_DISPLAY_OFFSET_Y					2
#define ENEMY_LEVEL_CONTAINER_MARGIN_X					2
#define ENEMY_LEVEL_CONTAINER_MARGIN_Y					2

//ITEM STATS DISPLAY
#define ITEM_STATS_PANEL_SIZE_X							320
#define ITEM_STATS_PANEL_SIZE_Y							358
#define ITEM_STATS_PANEL_POSITION_X						20
#define ITEM_STATS_PANEL_POSITION_Y						80

//STELLAR MAP
#define STELLARMAP_PANEL_WIDTH							1080
#define STELLARMAP_PANEL_HEIGHT							1080

//STELLAR MAP INFO PANEL
#define STELLARMAP_INFO_PANEL_SIZE_X					260
#define STELLARMAP_INFO_PANEL_SIZE_Y					70
#define STELLARMAP_INFO_PANEL_POSITION_X				20
#define STELLARMAP_INFO_PANEL_POSITION_Y				0
#define STELLARMAP_TELEPORTATION_COST					100
#define STELLARMAP_SCROLLING_SPEED						600
#define STELLARMAP_SCROLLING_MIN_SPEED					100
#define STELLARMAP_SCROLLING_AREA_WIDTH					24
#define STELLARMAP_MARGIN_SIDES_WIDTH					64
#define STELLARMAP_REAL_SIZE_X							500
#define STELLARMAP_REAL_SIZE_Y							500
#define STELLARMAP_SHIP_MINIATURE_SCALE					0.75

//EQUIPMENT GRIDS
#define SHIP_GRID_NB_LINES								1
#define SHIP_GRID_NB_ROWS								5
#define SHIP_GRID_SLOT_SIZE								64
#define SHIP_GRID_OFFSET_POS_X							20
#define SHIP_GRID_OFFSET_POS_Y							472

#define EQUIPMENT_GRID_NB_LINES							6
#define EQUIPMENT_GRID_NB_ROWS							5
#define EQUIPMENT_GRID_SLOT_SIZE						64
#define EQUIPMENT_GRID_OFFSET_POS_X						20
#define EQUIPMENT_GRID_OFFSET_POS_Y						578

#define SHOP_GRID_NB_LINES								6
#define SHOP_GRID_NB_ROWS								5
#define SHOP_GRID_SLOT_SIZE								64
#define SHOP_GRID_OFFSET_POS_X							20
#define SHOP_GRID_OFFSET_POS_Y							30

//ENEMY GENERATION
#define SPAWN_NORMAL_MINIMUM_RESOURCE					-0.05f
#define SPAWN_NORMAL_MAXIMUM_RESOURCE					0.05f
#define SPAWN_REPEAT_MINIMUM_RESOURCE					0.60f
#define SPAWN_REPEAT_MAXIMUM_RESOURCE					0.90f
#define SPAWN_MISS_MINIMUM_RESOURCE						0.40f
#define SPAWN_MISS_MAXIMUM_RESOURCE						0.70f

//DROP SYSTEM VARIABLES
#define NUMBER_OF_BEASTSCALE_THRESHOLD					5
#define NB_MAX_PROPERTIES_FOR_NON_EPIC_EQUIPMENT		3

#define BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES			3.0f
#define CREDITS_COST_PER_ONE_MULTISHOT					10
#define COST_PER_ONE_MULTISHOT_MULTIPLIER_PER_LEVEL		0.4
#define MIN_VALUE_OF_MULTISHOT							2
#define MAX_RATE_OF_FIRE_BONUS							90
#define MIN_WEAPON_XSPREAD								10

//HYPERSPEED
#define HYPERSPEED_CONSUMPTION_FOR_CRUISING				1
#define HYPERSPEED_CONSUMPTION_FOR_SLOWMOTION			1

//LEVELING
#define FIRST_LEVEL_MAX									20
#define XP_MAX_FIRST_LEVEL								100
#define XP_MAX_INCREASE_PER_LEVEL						0.50
#define XP_DEATH_MALUS_PERCENTAGE						0.05
#define PLAYER_LEVELING_STATS_MULTIPLIER				0.10
#define ENEMIES_LEVELING_STATS_MULTIPLIER				0.12
#define BOT_STATS_MULTIPLIER							0.35

#define LOOT_CREDITS_DEFAULT_BONUS						1
#define FIRST_LEVEL_ARMOR								100
#define FIRST_LEVEL_SHIELD								100
#define FIRST_LEVEL_SHIELD_REGEN						10
#define FIRST_LEVEL_SHIELD_RECOVERY_TIME				20.0f
#define FIRST_LEVEL_COLLISION_DAMAGE					10
#define FIRST_LEVEL_AMMO_DAMAGE							5
#define FIRST_LEVEL_MULTISHOT							2
#define FIRST_LEVEL_RATE_OF_FIRE						0.1f
#define FIRST_LEVEL_HYPERSPEED							1.0f
#define FIRST_LEVEL_FUEL								100
#define DEFAULT_AMMO_SPEED								2200
#define ASSUMED_SHIP_SIZE								70
#define ASSUMED_BOT_SIZE								32
#define WEAPON_MIN_VSPEED_VALUE							300

//TEXTPOP FEEDBACK
#define MONEY_LOOT_DISPLAY_NOT_FADED_TIME				0.8f
#define MONEY_LOOT_DISPLAY_FADE_OUT_TIME				1.f
#define MONEY_LOOT_DISPLAY_SPEED_Y						50
#define TEXT_POP_OFFSET_Y								10

#define SCENE_TITLE_OFFSET_Y							150
#define SCENE_TILE_FADE_IN_TIME							1.0f
#define SCENE_TILE_NOT_FADED_TIME						2.0f
#define SCENE_TILE_FADE_OUT_TIME						1.0f

#define DESTRUCTIONS_DISPLAY_OFFSET_Y					450
#define DESTRUCTIONS_DISPLAY_FADE_IN_TIME				0.5f
#define DESTRUCTIONS_DISPLAY_NOT_FADED_TIME				5.0f
#define DESTRUCTIONS_DISPLAY_FADE_OUT_TIME				2.0f

enum HazardLevels
{
	HAZARD_LEVEL_1,//0
	HAZARD_LEVEL_2,//1
	HAZARD_LEVEL_3,//2
	HAZARD_LEVEL_4,//3
	HAZARD_LEVEL_5,//4
	NB_HAZARD_LEVELS,//5
};

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

#endif // GLOBALS_H_INCLUDED