#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Weapon.h"

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
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_enemy_type=0);
	void update(sf::Time deltaTime) override;

	Weapon* weapon;
	int enemy_type;
	float probability;

	Enemy* Clone();
private:

};

#endif // INDEPENDANT_H_INCLUDED