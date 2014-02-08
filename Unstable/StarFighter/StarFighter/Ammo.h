#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "Independant.h"

enum AmmoType {
	LaserBeam,
	LaserBeamBlue,
	NBVAL_Ammo
};

class Ammo : public Independant
{

public:
	Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage, int m_ammo_type=0);
	int ammo_type;
	Ammo* Clone();

	//void update(sf::Time deltaTime) override;
};

#endif // AMMO_H_INCLUDED