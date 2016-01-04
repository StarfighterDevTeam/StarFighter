#ifndef GLOW_H_INCLUDED
#define GLOW_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Glow : public GameObject
{
public :
	Glow(GameObject* parent, sf::Color color);
	Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size = 0);
	Glow(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size, float glow_animation_duration, int glow_min_thickness);
	Glow(GameObject* parent, PlayerTeams team, int glow_thickness, int stroke_size, float glow_animation_duration, int glow_min_thickness);
	static sf::Uint8* CreateGlowFrame(GameObject* parent, sf::Color color, int glow_thickness, int stroke_size = 0);
	virtual ~Glow();
	void update(sf::Time deltaTime) override;

	sf::Color m_color;
	Uint8 m_glow_radius;
	float m_glow_animation_duration;
};

#endif // GLOW_H_INCLUDED
