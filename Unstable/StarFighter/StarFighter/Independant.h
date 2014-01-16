#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"

class Independant : public sf::Sprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);

	virtual void update(sf::Time deltaTime);

private:
	sf::Vector2f speed;
};


#endif // INDEPENDANT_H_INCLUDED