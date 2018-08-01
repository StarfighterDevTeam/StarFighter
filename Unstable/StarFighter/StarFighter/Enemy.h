#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Ship;

class Enemy : public GameObject
{
public :
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	~Enemy();
	void update(sf::Time deltaTime) override;

	int m_hp;
};

#endif // ENEMY_H_INCLUDED
