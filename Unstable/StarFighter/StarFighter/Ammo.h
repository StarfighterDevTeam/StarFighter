#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "GameObject.h"
#include "FX.h"
#include "Game.h"

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
};

#endif // AMMO_H_INCLUDED