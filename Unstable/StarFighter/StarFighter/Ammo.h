#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"

class Weapon;

enum AmmoType
{
	Ammo_LaserGreen,
};

class Ammo : public GameObject
{
public:
	Ammo(AmmoType ammo_type, sf::Vector2f position, float heading, float range, ColliderType collider, GameObject* locked_target);
	~Ammo();

	AmmoType m_ammo_type;

	float m_speed_max;
	float m_acceleration;
	float m_lifespan;
	GameObject* m_locked_target;

	void Update(sf::Time deltaTime) override;
};

#endif // AMMO_H_INCLUDED
