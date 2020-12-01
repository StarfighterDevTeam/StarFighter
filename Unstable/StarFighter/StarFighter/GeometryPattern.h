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
	Circle_,
	Rectangle_,
	NBVAL_PatternType
};

enum GeometryPatternData
{
	GEOMETRY_PATTERN_TYPE,//0
	GEOMETRY_PATTERN_SPEED,//1
	GEOMETRY_PATTERN_CLOCKWISE,//2
	GEOMETRY_PATTERN_STARTING_POINT,//3
	GEOMETRY_PATTERN_WIDTH,//4
	GEOMETRY_PATTERN_HEIGHT,//5
};

class GeometryPattern
{
public:
	GeometryPattern();

	static GeometryPattern* LoadPattern(vector<string> line_data, int index);
	void setPattern_v2(GeometryPattern* pattern);
	void setPattern_v2(PatternType pattern_type, int speed, int clockwise, int width, int height, int starting_point);
	void resetPattern();
	sf::Vector2f getOffset_v2(sf::Time deltaTime, bool global_offset = false);

	PatternType m_pattern_type;
	int m_speed;
	int m_clockwise;
	int m_width;
	int m_height;
	int m_starting_point;

private:
	sf::Vector2i m_direction;
	float m_distance_left;
	float m_theta;
	sf::Vector2f m_offset;
};


#endif // GeometryPattern_H_INCLUDED