#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Ammo.h"

class Ship;

enum WeaponType
{
	Weapon_Laser,
};

class Weapon
{
public:
	Weapon(GameObject* owner, WeaponType weapon_type, AmmoType ammo_type, ColliderType collider, LayerType layer, sf::Vector2f polar_offset = sf::Vector2f(0, 0), float heading_offset = 0);
	~Weapon();

	void Fire();
	void Update(sf::Time deltaTime);
	bool IsReadyToFire();
	bool CanStayLocked(GameObject* object);

	GameObject* m_owner;
	sf::Vector2f m_position;
	float m_heading;
	WeaponType m_weapon_type;
	AmmoType m_ammo_type;
	ColliderType m_collider;
	LayerType m_layer;
	sf::Vector2f m_polar_offset;//angle in degrees and length to center
	float m_heading_offset;
	float m_rate_of_fire;
	float m_rate_of_fire_timer;
	float m_range;
	GameObject* m_locked_target;
	float m_locking_target_clock;
	float m_locking_angle_coverage;

	RectangleShape m_lock_rectangle;
};

#endif // WEAPON_H_INCLUDED
