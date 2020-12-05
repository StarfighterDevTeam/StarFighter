#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include "Grid.h"

class Loot : public GameObject
{
public:
	Loot (sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, std::string display_name);
	void update(sf::Time deltaTime, float hyperspeedMultiplier);
	Loot* Clone();
};

#endif // LOOT_H_INCLUDED