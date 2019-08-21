#ifndef STAR_H_INCLUDED
#define STAR_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Star : public GameObject
{
public :
	Star(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float pulse_periodicity = 0.f);
	void Update(sf::Time deltaTime);

	float m_pulse_periodicity;
	float m_pulse_clock;
	sf::Color m_color;
};

#endif // STAR_H_INCLUDED
