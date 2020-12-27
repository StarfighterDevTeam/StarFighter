#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include "Shop.h"

struct Background : public GameObject
{
public:
	Background(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	~Background();

	Portal* m_portals[NO_DIRECTION];
	Shop* m_shop;

	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void SetPortalsState(PortalState state);
};

#endif // BACKGROUND_H_INCLUDED
