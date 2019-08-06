#ifndef WAVE_H_INCLUDED
#define WAVE_H_INCLUDED

#include "Game.h"

class Wave : public CircleObject
{
public :
	Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed);
	~Wave();

	void update(sf::Time deltaTime) override;

	float m_expansion_speed;
};

#endif // WAVE_H_INCLUDED
