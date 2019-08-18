#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Ballistic.h"

enum WeaponType
{
	Weapon_AAM,
};

class Weapon
{
public:
	Weapon(L16Entity* owner, WeaponType weapon_type, BallisticType ballistic_type, sf::Vector2f polar_offset = sf::Vector2f(0, 0), float heading_offset = 0);
	~Weapon();

	void Fire();
	void update(sf::Time deltaTime);
	bool IsReadyToFire();
	bool CanStayLocked(L16Entity* entity);

	sf::Vector2f m_position;
	float m_heading;
	WeaponType m_weapon_type;
	CircleType m_collision_domain;
	BallisticType m_ballistic_type;
	sf::Vector2f m_polar_offset;//angle and length to center
	float m_heading_offset;
	L16Entity* m_owner;
	float m_rate_of_fire;
	float m_rate_of_fire_timer;
	float m_range;
	L16Entity* m_locked_target;
	float m_locking_target_clock;
	float m_locking_angle_coverage;

	RectangleShape m_lock_rectangle;
};

#endif // WEAPON_H_INCLUDED
