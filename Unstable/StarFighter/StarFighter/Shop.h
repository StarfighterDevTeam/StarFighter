#ifndef SHOP_H_INCLUDED
#define SHOP_H_INCLUDED

#include "HudGrid.h"
#include "Independant.h"

class Shop : public Independant
{
public:
	Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;

	ObjectGrid fakeShopGrid;
	ObjectGrid shopGrid;
};

#endif // SHOP_H_INCLUDED