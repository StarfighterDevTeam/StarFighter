#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "Weapon.h"

#define	ENEMYX_DAMAGE			10
#define	ENEMYX_ARMOR			200
#define	ENEMYX_SHIELD			100
#define	ENEMYX_SHIELD_REGEN		1

class Enemy : public Independant
{

public:
	Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage);
	void update(sf::Time deltaTime) override;
private:
	Weapon weapon;
};

#endif // INDEPENDANT_H_INCLUDED