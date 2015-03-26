#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "FX.h"
#include "Game.h"

enum AmmoType {
	LaserBeam,
	LaserBeamBlue,
	NBVAL_Ammo
};

class Ammo : public Independant
{
public:
	Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, FX* m_explosion);
	Ammo* Clone();
	FX* explosion;
	void Death() override;
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;

	float radius;
	float angspeed;
	float ref_speed;
	float range;
	float current_range;
	float shot_angle;
};

#endif // AMMO_H_INCLUDED