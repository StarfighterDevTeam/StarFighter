#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "TextureLoader.h"
#include "TextUtils.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace sf;

std::string makePath(const std::string& srcPath);
void createSavesDirectory();
const char* getSavesPath();
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
std::string StringCut(std::string str, const size_t length);

struct Vector2int
{
	int x;
	int y;	
};

#ifdef __APPLE__
	#define PLAYER_SAVE_FILE						"SaveFile.txt"
#else
	#define PLAYER_SAVE_FILE						"Saves/SaveFile.txt"
#endif

#define	REF_WINDOW_RESOLUTION_X					1920.f
#define REF_WINDOW_RESOLUTION_Y					1080.f

#define	WINDOW_RESOLUTION_X						1600.f
#define WINDOW_RESOLUTION_Y						900.f

#define STARTSCENE_X_RATIO						0.5f
#define STARTSCENE_Y_RATIO						0.85f

#define ICON_SHIP_PNG							"Icons/ShipModel_icon.png"

#define STAR_SECTOR_SIZE						300

#define	GHOST_ALPHA_VALUE						120

#define JOYSTICK_MIN_AXIS_VALUE					25

#define TEXTURE_SIZE_LIMITATION						8187

#define PIXEL_PERFECT_COLLISION							true
#define GRAZE_DISTANCE									30
#define RECTANGLE_INSIDE_ALPHA							100

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
int RandomizeIntBetweenFloats(float min_value, float max_value);
float RandomizeFloatBetweenValues(float min_value, float max_value);
int RandomizeSign();
float MaxBetweenValues(float x, float y);
float MinBetweenValues(float x, float y);
float Lerp(float value, float input_min, float input_max, float output_min, float output_max);
float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max);
sf::Color GrayScaleColor(sf::Color input_color, float ratio);
float GetAngleRadFromVector(sf::Vector2f vector);
float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
bool NormalizeVector(sf::Vector2f* vector, float max_value);
void ScaleVector(sf::Vector2f* vector, float target_value);
void ScaleVectorInt(sf::Vector2i* vector, int target_value);
float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
float GetDistanceSquaredBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
float GetVectorLength(sf::Vector2f vector);
float GetVectorLengthSquared(sf::Vector2f vector);
void Bound(float& input, float min, float max);
void BoundAbsoluteValue(float& input, float max_value);
void BoundAngle(float& input, float max_angle);
bool IsInsideArea(sf::Vector2f bounds, sf::Vector2f coordinates, sf::Vector2f area_size);
void DebugDrawSegment(sf::Vector2f point_a, sf::Vector2f point_b, sf::Color color, RenderTarget& screen);
sf::Vector2f GetVectorFromLengthAndAngle(const float length, const float angle);

struct Vector2iComp
{
	bool operator() (sf::Vector2i l, sf::Vector2i r) { return (l.x < r.x || l.x == r.x && l.y < r.y);; }
};

#endif // GLOBALS_H_INCLUDED