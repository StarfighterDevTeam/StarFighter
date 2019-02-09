#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace sf;

std::string makePath(const std::string& srcPath);
void createSavesDirectory();
const char* getSavesPath();
std::string StringReplace(std::string str, const std::string& from, const std::string& to);

#ifdef __APPLE__
	#define PLAYER_SAVE_FILE						"SaveFile.txt"
#else
	#define PLAYER_SAVE_FILE						"Saves/SaveFile.txt"
#endif

#define	REF_WINDOW_RESOLUTION_X					1920
#define REF_WINDOW_RESOLUTION_Y					1080

#define	WINDOW_RESOLUTION_X						1600
#define WINDOW_RESOLUTION_Y						900

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define ICON_SHIP_PNG							"Icons/ShipModel_icon.png"

#define	GHOST_ALPHA_VALUE						120

#define JOYSTICK_MIN_AXIS_VALUE					25

#define TEXTURE_SIZE_LIMITATION						8187

#define PIXEL_PERFECT_COLLISION							true
#define GRAZE_DISTANCE									30
#define RECTANGLE_INSIDE_ALPHA							100

//PIRATES
#define ROOMTILE_SIZE				48.f
#define ROOMTILE_OFFSET_X			450.f
#define ROOMTILE_OFFSET_Y			550.f

#define ROOMTILE_MINI_OFFSET_X		1600.f
#define ROOMTILE_MINI_OFFSET_Y		(ROOMTILE_OFFSET_Y	)

#define DISTANCE_COMBAT_INIT				1000.f
#define WARP_AMMO_OFFSET_X					1000.f

#define ROOMTILE_HEALTH_MAX					10
#define ROOMTILE_FLOODING_MAX				100
#define ROOMTILE_FLOODING_GENERATION		1
#define FLOODING_TIMER						0.15f
#define FLOOD_MIN_VALUE_FOR_TRANSFER		20
#define FLOOD_EVAPORATION_TIME				1.f

#define WATERTILE_SIZE						64.f
#define WATERTILE_OFFSET_X					1100.f
#define WATERTILE_OFFSET_Y					150.f
#define NB_WATERTILE_X						60
#define NB_WATERTILE_Y						60
#define NB_WATERTILE_SUBDIVISION			60
#define NB_WATERTILE_VIEW_RANGE				5 // the eye can see ~8km at 10m altitude, and ~11km at 15m (because of Earth curve). Hence ~5 nautical miles away (nm) = 5 latitude minutes

#define HULL_REPAIR_TIMER					1.f
#define DROWNING_TIMER						1.f
#define HEALING_TIMER						1.f

#define AI_CHANGE_TARGETROOM_PERCENTAGE		0.15f

#define NAVIGATION_DODGE_CHANCE				0.1f
#define DODGE_CHANCE_HARDCAP				0.75f

#define ENGINE_DODGE_CHANCE					0.05f
#define ENGINE_ROTATION_SPEED				10.f
#define ENGINE_FLEE_ROTATION_SPEED			20.f
#define ENGINE_FLEE_COUNT					10.f
#define SHIP_FLEE_SPEED_MAX					20.f
#define SHIP_FLEE_ACCELERATION				1.f

#define RUDDER_ROTATION_TIMER_MIN			2.f
#define RUDDER_ROTATION_TIMER_MAX			7.f
#define RUDDER_ROTATION_SPEED_MIN			0.1f
#define RUDDER_ROTATION_SPEED_MAX			0.4f

#define ENGINE_HEALTH_MAX					50

#define LIFEBAR_SIZE_X						20.f
#define LIFEBAR_SIZE_Y						4.f
#define LIFEBAR_OFFSET_Y					8.f

//MUSIC
#define DEFAULT_MUSIC_VOLUME				50
#define DEFAULT_SFX_VOLUME					100
#define MUSIC_FADE_IN_TIME					2.f
#define MUSIC_FADE_OUT_TIME					2.f

//TEXT POP FEEDBACK
#define TEXT_POP_OFFSET_Y							10

#define INTERACTION_BUTTON_A_FILENAME			"2D/HUD/Button_A.png"
#define INTERACTION_BUTTON_X_FILENAME			"2D/HUD/Button_X.png"
#define INTERACTION_BUTTON_Y_FILENAME			"2D/HUD/Button_Y.png"
#define INTERACTION_BUTTON_B_FILENAME			"2D/HUD/Button_B.png"
#define INTERACTION_BUTTON_WIDTH				16
#define INTERACTION_BUTTON_HEIGHT				16
#define INTERACTION_BUTTON_MARGIN				4
#define INTERACTION_INTERBLOCK					20
#define INTERACTION_MARGIN_SIDES				12
#define SFPANEL_SPECIFIC_WIDTH					200
#define SFPANEL_SPECIFIC_HEIGHT					100

//CSV DATA
#define SHIP_CSV_FILE							"Scripts/Generated/Ship.csv"

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values);
float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios);
float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold);
int RandomizeIntBetweenValues(int min_value, int max_value);
int RandomizeIntBetweenFloats(sf::Vector2f min_max_values);
float RandomizeFloatBetweenValues(float min, float max);
int RandomizeSign();
void Bound(float& value, sf::Vector2f min_max_values);
void Bound(int& value, sf::Vector2i min_max_values);
int Min(int a, int b);
int Max(int a, int b);
float Minf(float a, float b);
float Maxf(float a, float b);
float Lerp(float value, float input_min, float input_max, float output_min, float output_max);
float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max);
sf::Color GrayScaleColor(sf::Color input_color, float ratio);
void ScaleVector(sf::Vector2f* vector, float target_value);
void GetAngleForVector(sf::Vector2f vector, float& angle);

#endif // GLOBALS_H_INCLUDED