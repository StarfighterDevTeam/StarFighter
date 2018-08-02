#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Loot : public GameObject
{
public :
	Loot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	void update(sf::Time deltaTime) override;

	sf::Vector2f m_melee_range_bonus;
};

#endif // LOOT_H_INCLUDED
