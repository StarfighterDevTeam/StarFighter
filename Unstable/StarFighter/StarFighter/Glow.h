#ifndef GLOW_H_INCLUDED
#define GLOW_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Glow : public GameObject
{
public :
	Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size = 0);
	virtual ~Glow();
	void update(sf::Time deltaTime) override;

	sf::Color m_color;
	Uint8 m_glow_radius;
	sf::Clock m_glow_variation_clock;
};

#endif // GLOW_H_INCLUDED
