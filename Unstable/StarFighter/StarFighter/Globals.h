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
	#define EDITOR_SAVE_FILE						"SaveFile.txt"
#else
	#define EDITOR_SAVE_FILE						"Saves/EditorFile.txt"
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

//TILES
#define TILE_SIZE				64;
//#define NB_TILE_LINES			8;
//#define NB_TILE_ROWS			9;

//PATHFIND
#define ANGLE_UPDATE_TIME							0.5f

//AGENT
#define MAX_ITEMS_PER_AGENT							5
#define WALKER_IDLE_MIN_WAIT_TIME					1.f
#define WALKER_IDLE_MAX_WAIT_TIME					5.f
#define WALKER_MIN_WALK_TIME						119.f
#define WALKER_MAX_WALK_TIME						120.f
#define WALKER_MIN_SPEED							150.f
#define WALKER_MAX_SPEED							250.f
#define WALKER_ANGULAR_SPEED						50.f
#define WALKER_SPEED_UP_VARIANCE					1.30f
#define WALKER_SLOW_DOWN_VARIANCE					0.70f

#define WALKER_CHANCE_OF_WALKING					0.60f

#define WALKER_AWARENESS_PER_CODE					0.10f
#define WALKER_AWARENESS_RAISE_ON_COLLISION			0.25f
#define WALKER_AWARENESS_RAISE_ON_SIGHT				0.02f
#define WALKER_AWARENESS_MAX_ON_SIGHT				0.30f
#define WALKER_AWARENESS_REDUCTION_ON_IDLE			0.03f

#define WALKER_AWARENESS_THRESHOLD_CASUAL			0.1f
#define WALKER_AWARENESS_THRESHOLD_SUSPICIOUS		0.3f
#define WALKER_AWARENESS_THRESHOLD_ANGRY			0.8f
#define WALKER_AWARENESS_THRESHOLD_MAD				1.0f

//ASSETS
#define ITEM_SIZE								32
#define ITEM_NAME_SIZE							128
#define CODE_SIZE								32

//MUSIC
#define DEFAULT_MUSIC_VOLUME						50
#define DEFAULT_SFX_VOLUME							100
#define MUSIC_FADE_IN_TIME						2.f
#define MUSIC_FADE_OUT_TIME						2.f

//TEXT POP FEEDBACK
#define TEXT_POP_OFFSET_Y							10

#define CODE_BUTTON_ARROW						"2D/arrow.png"
#define INTERACTION_BUTTON_A_FILENAME			"2D/HUD/Button_A.png"
#define INTERACTION_BUTTON_X_FILENAME			"2D/HUD/Button_X.png"
#define INTERACTION_BUTTON_Y_FILENAME			"2D/HUD/Button_Y.png"
#define INTERACTION_BUTTON_B_FILENAME			"2D/HUD/Button_B.png"
#define INTERACTION_BUTTON_WIDTH				16
#define INTERACTION_BUTTON_HEIGHT				16
#define INTERACTION_BUTTON_MARGIN				4
#define INTERACTION_INTERBLOCK					20
#define INTERACTION_MARGIN_SIDES				12
#define SFSTRATAGEM_PANEL_WIDTH					390
#define SFSTRATAGEM_PANEL_HEIGHT				130
#define SFSTRATAGEM_PANEL_AWARENESS_HEIGHT		14

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