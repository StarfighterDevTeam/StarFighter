#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"

class Weapon;

enum AmmoType
{
	Ammo_LaserGreen,
	Ammo_LaserRed,
	Ammo_Missile,
};

class Ammo : public GameObject
{
public:
	Ammo(AmmoType ammo_type, sf::Vector2f position, float heading, float range, ColliderType collider, GameObject* locked_target);
	~Ammo();

	AmmoType m_ammo_type;

	float m_acceleration;
	float m_speed_min;
	float m_speed_max;
	float m_turn_speed;
	float m_lifespan;
	float m_radar_homing_range;
	GameObject* m_locked_target;

	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
};

#endif // AMMO_H_INCLUDED
