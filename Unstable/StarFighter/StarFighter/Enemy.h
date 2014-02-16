#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Weapon.h"
#include "Globals.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "MovePattern.h"

#define	ENEMYX_DAMAGE			5
#define	ENEMYX_ARMOR			100
#define	ENEMYX_SHIELD			100
#define	ENEMYX_SHIELD_REGEN		1

#define	ENEMYY_DAMAGE			5
#define	ENEMYY_ARMOR			200
#define	ENEMYY_SHIELD			200
#define	ENEMYY_SHIELD_REGEN		1

enum EnemyType {
	EnemyX,
	EnemyY,
	NBVAL_Enemy
};

class Enemy : public Independant
{

public:
	Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death);
	void update(sf::Time deltaTime, sf::Vector2f polarOffset[MovePatternType::NBVAL_MovePattern]) override;
	void update(sf::Time deltaTime) override;
	Weapon* weapon;
	Enemy* Clone();
	FX* FX_death;
	Independant* death();
	int movepattern_type;
	
private:
		
};


#endif // INDEPENDANT_H_INCLUDED