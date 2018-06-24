#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include "Logger.h"
#include <assert.h>
#include <math.h>

std::string makePath(const std::string& srcPath);
void createSavesDirectory();
const char* getSavesPath();
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

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

//SCREEN BORDERS
#define AVOID_BORDERS_RADIUS_COEFF			1.0f

//FLOCKING
#define FLOCKING_RADIUS						100
#define FLOCKING_BASE_SPEED_MIN				210
#define FLOCKING_BASE_SPEED_MAX				210

#define FLOCKING_PREVIOUS_SPEED_WEIGHT		5.0f
#define FLOCKING_COHESION_WEIGHT			0.8f
#define FLOCKING_ALIGNMENT_WEIGHT			0.15f
#define FLOCKING_SEPARATION_WEIGHT			2.0f
#define FLOCKING_AVOID_BORDERS_WEIGHT		200.0f

//FLEEING
#define FLEEING_MAX_SPEED					500
#define FLEEING_RADIUS						100.f
#define FLEEING_ANGLE						120
#define FLEEING_DURATION					0.8f

#define FLOCKING_SEPARATION_RADIUS			50

//BOIDS
#define BOID_MIN_CHANGE_DIR_TIME			1.f
#define BOID_MAX_CHANGE_DIR_TIME			4.f
#define BOID_MIN_CHANGE_DIR_ANGLE			20.f
#define BOID_MAX_CHANGE_DIR_ANGLE			90.f

//PREDATORS
#define PREDATOR_MIN_CHANGE_DIR_TIME		5.f
#define PREDATOR_MAX_CHANGE_DIR_TIME		15.f
#define PREDATOR_MIN_CHANGE_DIR_ANGLE		30.f
#define PREDATOR_MAX_CHANGE_DIR_ANGLE		150.f

#define PREDATOR_AVOID_BORDERS_WEIGHT		50.0f

#define PREDATOR_SCALE						4.f

#define PREDATOR_BASE_SPEED_MIN				230
#define PREDATOR_BASE_SPEED_MAX				230

#define PREDATOR_CHASING_ANGLE				70.f
#define PREDATOR_CHASING_RADIUS				200.f
#define PREDATOR_ATTACK_DURATION			0.4f
#define PREDATOR_ATTACK_COOLDOWN			1.0f
#define PREDATOR_ATTACK_RADIUS				80.f
#define PREDATOR_ATTACK_SPEED				1000
#define PREDATOR_EATING_RADIUS				0.f
#define PREDATOR_EATING_COOLDOWN			0.5f

//EGGS AND BABIES
#define EGG_LAYING_COOLDOWN						10.f
#define EGG_LAYING_CHANCE						0.30f//0.05f
#define EGG_NB_MIN								3
#define EGG_NB_MAX								8
#define EGG_LAYING_TIME_BETWEEN_TWO_EGGS		1.f
#define BABY_BOID_SCALE							0.5f
#define BABY_GROWTH_DURATION					20.f

//MUSIC
#define DEFAULT_MUSIC_VOLUME						50
#define DEFAULT_SFX_VOLUME							100
#define MUSIC_FADE_IN_TIME						2.f
#define MUSIC_FADE_OUT_TIME						2.f

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
float RandomizeFloatBetweenValues(sf::Vector2f min_max_values);
float MaxBetweenValues(sf::Vector2f values);
float MinBetweenValues(sf::Vector2f values);

#endif // GLOBALS_H_INCLUDED