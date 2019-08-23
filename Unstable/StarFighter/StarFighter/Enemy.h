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
	Enemy(EnemyType enemy_type, sf::Vector2i sector_index, float heading);
	void Update(sf::Time deltaTime) override;

	EnemyType m_enemy_type;
};

#endif // ENEMY_H_INCLUDED
