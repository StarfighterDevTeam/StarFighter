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

#define GAME_SPEED			500.f

using namespace sf;

std::string makePath(const std::string& srcPath);
void createSavesDirectory();
const char* getSavesPath();
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
std::string StringCut(std::string str, const size_t length);

#ifdef __APPLE__
	#define PLAYER_SAVE_FILE						"SaveFile.txt"
#else
	#define PLAYER_SAVE_FILE						"Saves/SaveFile.txt"
#endif

#define M_PI_FLT       3.14159265358979323846f   // pi
#define M_PI_2_FLT     1.57079632679489661923f   // pi/2
#define M_PI_4_FLT     0.785398163397448309616f  // pi/4

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
#define GRAZE_DISTANCE									30.f
#define RECTANGLE_INSIDE_ALPHA							100

//MUSIC
#define DEFAULT_MUSIC_VOLUME						50.f
#define DEFAULT_SFX_VOLUME							100.f
#define MUSIC_FADE_IN_TIME							2.f
#define MUSIC_FADE_OUT_TIME							2.f

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
int RandomizeSign();
float MaxBetweenValues(sf::Vector2f values);
float MinBetweenValues(sf::Vector2f values);
float Lerp(float value, float input_min, float input_max, float output_min, float output_max);
float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max);
sf::Color GrayScaleColor(sf::Color input_color, float ratio);
bool IsInsideAngleCoverage(float angle_input, float angle_coverage, float angle_target);
float Bound(const float input, float min, float max);
void BoundAngle(float& input, float max_angle);
bool IsInsideArea(sf::Vector2f bounds, sf::Vector2f coordinates, sf::Vector2f area_size);

float GetVectorLength(sf::Vector2f vector);
float GetVectorLengthSquared(sf::Vector2f vector);
sf::Vector2f GetVectorFromLengthAndAngle(float length, float angle);
bool NormalizeVector(sf::Vector2f* vector, float max_value);
bool BoundVector(sf::Vector2f* vector, float min_value, float max_value);
void ScaleVector(sf::Vector2f* vector, float target_value);
void AddValueToVector(sf::Vector2f* vector, float added_value);
float GetAngleRadForVector(sf::Vector2f vector);
void GetAngleRadAndLengthOfVector(sf::Vector2f vector, float* output_length, float* output_angle);
float GetAngleRadToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
float GetAngleDegToTargetPosition(sf::Vector2f ref_position, float ref_rotation_in_deg, sf::Vector2f target_position);
float GetDistanceBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
float GetDistanceSquaredBetweenPositions(sf::Vector2f position1, sf::Vector2f position2);
float GetAngleRadBetweenPositions(sf::Vector2f ref_position, sf::Vector2f position2);
sf::Vector2f RotateVector(sf::Vector2f vector, float angle_rad);
void SetVectorRotation(sf::Vector2f* vector, float angle_rad);
bool IntersectSegments(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x = NULL, float *i_y = NULL);
float DistancePointToSement(float p0_x, float p0_y, float p_1x, float p_1y, float p_2x, float p_2y, float *i_x = NULL, float *i_y = NULL);
float ComputeDeltaAngleInDegrees(const float angleDegreesA, const float angleDegreesB);

sf::Uint8* CreateRectangleWithStroke(sf::Vector2u size, sf::Color color, int stroke_size = 0);
sf::Uint8* CreateCircleWithStroke(int radius, sf::Color color, int stroke_size = 0);

int GetPixelDistanceFromEdge(int pixel_index, int width, int height);
void GlowEffect(int blur_radius, sf::Uint8* pixels, int width, int height, int stroke_size = 0);
int GaussianBlurDistribution(int x);

#endif // GLOBALS_H_INCLUDED