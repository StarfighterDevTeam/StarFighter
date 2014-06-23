#ifndef PATTERNBOBBY_H_INCLUDED
#define PATTERNBOBBY_H_INCLUDED

#include <string>
#include "IPatternHandler.h"
#include "TextUtils.h"

using namespace std;

enum PatternType 
{
	NoMovePattern,
	Rectangle_,
	NBVAL_PatternType
};


class PatternBobby : virtual public IPatternHandler
{

public:
	PatternBobby();
	void SetPattern(PatternType pt, int patternSpeed, vector<int>*  args);
	sf::Vector2f GetOffset(float seconds) override;

private:
	PatternType currentPattern;
	vector<int>*  patternParams;
	int patternSpeed;

	sf::Vector2i _direction;
	float _distance_left;

};


#endif // PATTERNBOBBY_H_INCLUDED