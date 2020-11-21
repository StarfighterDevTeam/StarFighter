#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "FX.h"

enum AmmoType {
	LaserBeam,
	LaserBeamBlue,
	NBVAL_Ammo
};

class Ammo : public GameObject
{
public:
	Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int damage, FX* explosion);
	Ammo* Clone();
	void Death() override;
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;

	FX* m_explosion;
	float m_radius;
	float m_angspeed;
	float m_ref_speed;
	float m_range;
	float m_current_range;
	float m_shot_angle;
	float m_offset_x;//offset respect to weapon on firing
	bool m_isBeam;
};

#endif // AMMO_H_INCLUDED