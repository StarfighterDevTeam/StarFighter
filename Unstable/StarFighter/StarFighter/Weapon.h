#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Ammo.h"

class Ship;

enum WeaponType
{
	Weapon_Laser,
	Weapon_Missile,
};

class Weapon
{
public:
	Weapon(SpatialObject* owner, WeaponType weapon_type, AmmoType ammo_type, ColliderType collider, LayerType layer, sf::Vector2f weapon_offset, float heading_offset = 0);
	~Weapon();

	void Fire();
	void Update(sf::Time deltaTime);
	bool IsReadyToFire();
	bool IsTargetAligned(SpatialObject* target, float angle_tolerance);

	SpatialObject* m_owner;
	sf::Vector2f m_position;
	float m_heading;
	WeaponType m_weapon_type;
	AmmoType m_ammo_type;
	ColliderType m_collider;
	LayerType m_layer;
	sf::Vector2f m_weapon_offset;
	float m_heading_offset;
	float m_rate_of_fire;
	float m_rate_of_fire_timer;
	float m_range;
	SpatialObject* m_locked_target;
	float m_locking_target_clock;
	int m_damage;
	int m_energy_cost;

	RectangleShape m_lock_rectangle;
};

#endif // WEAPON_H_INCLUDED
