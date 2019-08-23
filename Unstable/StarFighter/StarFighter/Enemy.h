#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Ship.h";

enum EnemyType
{
	Enemy_Alpha,
};

class Enemy : public Ship
{
public :
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Enemy(EnemyType enemy_type, sf::Vector2i sector_index, float heading);
	void Update(sf::Time deltaTime) override;

	EnemyType m_enemy_type;
};

#endif // ENEMY_H_INCLUDED
