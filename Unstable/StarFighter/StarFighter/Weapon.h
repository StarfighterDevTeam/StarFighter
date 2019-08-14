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
	Weapon(L16Entity* owner, WeaponType type, BallisticType ballistic_type, sf::Vector2f polar_offset = sf::Vector2f(0, 0), float heading_offset = 0);
	~Weapon();

	void Fire();

	WeaponType m_type;
	BallisticType m_ballistic_type;
	sf::Vector2f m_polar_offset;//angle and length to center
	float m_heading_offset;
	L16Entity* m_owner;
	//void update(sf::Time deltaTime) override;
};

#endif // WEAPON_H_INCLUDED
