#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"
#include "FX.h"

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
	Independant* explode();
	void update(sf::Time deltaTime) override;
	void update(sf::Time deltaTime, sf::Vector2f polarOffset[MovePatternType::NBVAL_MovePattern]) override;

	float ref_speed;
};

#endif // AMMO_H_INCLUDED