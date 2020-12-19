#ifndef SHOP_H_INCLUDED
#define SHOP_H_INCLUDED

#include "EnemyGenerator.h"

class Shop : public GameObject
{
public:
	Shop(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int m_frameNumber = 1, int m_animationNumber = 1);
	~Shop();
	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	int m_level;

	static int SaveShop(Shop* ship);
	static bool LoadShop(Shop* shop);

	static int SaveShopUpgrades(Shop* ship);
	static bool LoadShopUpgrades(Shop* shop);

	Grid* m_grid_v2;

	string m_upgrades[NB_UPGRADE_CHOICES];
	bool m_sold_out[NB_UPGRADE_CHOICES];
};

#endif // SHOP_H_INCLUDED