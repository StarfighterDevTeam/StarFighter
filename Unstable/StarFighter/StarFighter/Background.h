#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include "Portal.h"

struct Background : public Independant
{
public:
	Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, Directions direction, float first_screen_offset = 0);
	Portal* portals[Directions::NO_DIRECTION];
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void SetPortalsState(PortalState m_state);
};

#endif // BACKGROUND_H_INCLUDED
