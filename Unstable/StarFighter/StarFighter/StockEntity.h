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

	string GetBestPropulsionAvailable();
	static string GetBestPropulsionAvailable(map<string, size_t> fuel_tank);
	size_t LoadOre(string ore_name, size_t quantity);
	size_t Load(string ore_name, size_t quantity);
	virtual bool CanSupplyFuel();
	bool CanBeDrilled();
	size_t GetLightestOreWeight();
	string GetMostExpansiveOreAvailable();
	Ore* GetRandomOre();
	void UnloadCarriage(StockEntity* location);
	virtual size_t LoadFuel(string ore_name, size_t quantity);
	virtual size_t GetStockMax();
	virtual size_t GetFuelMax();

	string m_display_name;

	int m_stock_max;
	int m_stock;
	map<string, size_t> m_ores_stocked;
	size_t m_min_ore_weight;

	size_t m_fuel;
	size_t m_fuel_max;
	map<string, size_t> m_fuel_tanks;
	
	map<string, float> m_ore_presence_rates;
};

#endif // STARSHIP_H_INCLUDED
