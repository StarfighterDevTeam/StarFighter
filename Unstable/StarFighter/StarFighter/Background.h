#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include <SFML/Graphics.hpp>

#define BACKGROUND_FILENAME				"Assets/2D/background.png"
#define BACKGROUND_START_X				0
#define BACKGROUND_START_Y				0

struct Background : sf::Sprite
{
	sf::Vector2f speed;

    Background();
	void Init(int x, int y);
    
};

#endif // BACKGROUND_H_INCLUDED
