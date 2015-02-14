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
	Rectangle_,
	Circle_,
	Oscillator,
	NBVAL_PatternType
};


class PatternBobby : virtual public IPatternHandler
{

public:
	PatternBobby();
	void SetPattern(PatternType pt, float patternSpeed, vector<float>*  args);
	sf::Vector2f GetOffset(float seconds, bool absolute_coordinate = false) override;
	PatternType GetCurrentPatternType();

private:
	sf::Vector2f ToCartesianCoords(sf::Vector2f polarCoords);
	void ToCartesianCoords(sf::Vector2f* polarCoords);
	void CheckArgSize(int expected);

	float patternSpeed;
	PatternType currentPattern;
	vector<float>*  patternParams;
	sf::Vector2i _direction;
	sf::Vector2f _curSandboxPosition_cartesian;
	sf::Vector2f _curSandboxPosition_polar;

	float _distance_left;
	float _currTheta;

};


#endif // PATTERNBOBBY_H_INCLUDED