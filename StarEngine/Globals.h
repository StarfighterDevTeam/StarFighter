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

#define	GHOST_ALPHA_VALUE						120

#define JOYSTICK_MIN_AXIS_VALUE					25

#define TEXTURE_SIZE_LIMITATION						8187

#define PIXEL_PERFECT_COLLISION							true
#define GRAZE_DISTANCE									30

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