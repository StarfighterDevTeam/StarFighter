#ifndef SHOP_H_INCLUDED
#define SHOP_H_INCLUDED

#include "HudGrid.h"
#include "Game.h"
#include "GameObject.h"

class Shop : public GameObject
{
public:
	Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	~Shop();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	int m_level;
	vector<GameObject*> m_items;
};

#endif // SHOP_H_INCLUDED