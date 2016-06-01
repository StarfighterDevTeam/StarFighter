#ifndef STOCKENTITY_H_INCLUDED
#define STOCKENTITY_H_INCLUDED

#include "GameObject.h"
#include "Ore.h"

class StockEntity : public GameObject
{
public :
	StockEntity() {};
	StockEntity(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~StockEntity();

	void update(sf::Time deltaTime) override;

	virtual string GetBestPropulsionAvailable();
	size_t Stock(string ore_name, size_t quantity);
	virtual bool CanSupplyFuel();
	bool CanBeDrilled();
	size_t GetLightestOreWeight();
	Ore* GetRandomOre();

	string m_display_name;
	int m_stock_max;
	int m_stock;
	size_t m_min_ore_weight;

	map<string, size_t> m_ores_stocked;
	map<string, float> m_ore_presence_rates;
};

#endif // STARSHIP_H_INCLUDED
