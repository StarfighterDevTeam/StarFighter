#ifndef GeometryPattern_H_INCLUDED
#define GeometryPattern_H_INCLUDED

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <string>

#include "InputGuy.h"

using namespace std;

enum PatternType 
{
	NoMovePattern,
	Line_,
	Rectangle_,
	Circle_,
	Oscillator,
	NBVAL_PatternType
};

enum GeometryPatternData
{
	GEOMETRY_PATTERN_TYPE,//0
	GEOMETRY_PATTERN_SPEED,//1
	GEOMETRY_PATTERN_ARG1,//2
	GEOMETRY_PATTERN_ARG2,//3
	GEOMETRY_PATTERN_ARG3,//4
};

class GeometryPattern
{
public:
	GeometryPattern();
	void SetPattern(PatternType pt, float patternSpeed, vector<float> args);
	void StartPattern();
	sf::Vector2f GetOffset(float seconds, bool absolute_coordinate = false);
	static GeometryPattern* PatternLoader(vector<string> line_data, int index);

	PatternType m_currentPattern;
	vector<float> m_patternParams;
	float m_patternSpeed;
	float m_patternSpeedInRadian;

private:
	sf::Vector2f ToCartesianCoords(sf::Vector2f polarCoords);
	void ToCartesianCoords(sf::Vector2f* polarCoords);
	void CheckArgSize(size_t expected);

	sf::Vector2i m_direction;
	sf::Vector2f m_curSandboxPosition_cartesian;
	sf::Vector2f m_curSandboxPosition_polar;

	float m_distance_left;
	float m_currTheta;
};


#endif // GeometryPattern_H_INCLUDED