#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Marker.h"

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
	Ammo(AmmoType ammo_type, sf::Vector2f position, float heading, float range, int damage, ColliderType collider);
	~Ammo();

	AmmoType m_ammo_type;

	float m_acceleration;
	float m_speed_min;
	float m_speed_max;
	float m_turn_speed;
	float m_lifespan;
	float m_radar_homing_range;
	float m_radar_homing_angle_max;
	SpatialObject* m_locked_target;
	int m_damage;
	FXType m_FX_type;

	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void Bounce(GameObject* target);
};

#endif // AMMO_H_INCLUDED
