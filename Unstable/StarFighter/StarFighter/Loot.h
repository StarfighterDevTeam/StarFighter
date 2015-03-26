#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Ship.h"

class Loot : public Independant
{
public:
	Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, std::string m_display_name);
	void update(sf::Time deltaTime, float hyperspeedMultiplier);
	Loot* Clone();
};

#endif // LOOT_H_INCLUDED