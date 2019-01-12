#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define CANNONBALL_SIZE			16.f
#define CANNONBALL_SPEED		200.f

enum AmmoType
{
	Ammo_CannonBall,
};

class Ammo : public GameEntity
{
public:
	Ammo(AmmoType type);

	AmmoType m_type;

	float m_angle;
	sf::Vector2f m_speed;
	float m_ref_speed;

	void Update(Time deltaTime);
};

#endif //AMMO_H_INCLUDED