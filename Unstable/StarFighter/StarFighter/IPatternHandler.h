#ifndef IPATTERNHANDLER_H_INCLUDED
#define IPATTERNHANDLER_H_INCLUDED

#include <vector>
#include <SFML/Graphics.hpp>

using namespace std;


class IPatternHandler
{
public:
	virtual sf::Vector2f GetOffset(float seconds) = 0;
};

#endif