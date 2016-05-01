#ifndef PATTERNBOBBY_H_INCLUDED
#define PATTERNBOBBY_H_INCLUDED

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <string>
#include "IPatternHandler.h"
#include "TextUtils.h"
#include "Logger.h"

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

enum BobbyPatternData
{
	BOBBY_PATTERN_TYPE,//0
	BOBBY_PATTERN_SPEED,//1
	BOBBY_PATTERN_ARG1,//2
	BOBBY_PATTERN_ARG2,//3
	BOBBY_PATTERN_ARG3,//4
};

class PatternBobby : virtual public IPatternHandler
{

public:
	PatternBobby();
	void SetPattern(PatternType pt, float patternSpeed, vector<float> args);
	sf::Vector2f GetOffset(float seconds, bool absolute_coordinate = false) override;
	static PatternBobby* PatternLoader(vector<string> line_data, int index);

	PatternType m_currentPattern;
	vector<float> m_patternParams;
	float m_patternSpeed;

private:
	sf::Vector2f ToCartesianCoords(sf::Vector2f polarCoords);
	void ToCartesianCoords(sf::Vector2f* polarCoords);
	void CheckArgSize(size_t expected);

	sf::Vector2i m_direction;
	sf::Vector2f m_curSandboxPosition_cartesian;
	sf::Vector2f m_curSandboxPosition_polar;

	float m_distance_left;
	float m_currTheta;
	float m_firstLoop;
};


#endif // PATTERNBOBBY_H_INCLUDED