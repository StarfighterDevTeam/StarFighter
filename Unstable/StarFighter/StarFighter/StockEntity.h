#ifndef STOCKENTITY_H_INCLUDED
#define STOCKENTITY_H_INCLUDED

#include "GameObject.h"
#include "Ore.h"

class StockEntity : public GameObject
{
public :
	StockEntity() {};
	StockEntity(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	~StockEntity();

	void update(sf::Time deltaTime) override;

	size_t LoadInStock(string resource_name, size_t quantity);
	virtual bool CanSupplyFuel();
	bool CanBeDrilled();
	Ore* DigRandomOre(bool can_be_fuel = true, bool can_be_ore = true);
	void UnloadStocks(StockEntity* entity);
	size_t SupplyFuelTank(pair<string, size_t>& starship_fuel_tank, StockEntity& starship);
	virtual size_t LoadFuelTank(string fuel_name, size_t quantity);
	string GetBestPropulsionAvailable();
	virtual size_t GetOreMax();
	virtual size_t GetFuelMax();

	bool m_identified;

	string m_display_name;

	size_t m_ore_stock_max;
	size_t m_current_ore_stock;
	std::map<string, size_t> m_ore_stock;

	size_t m_fuel_stock_max;
	size_t m_current_fuel_stock;
	map<string, size_t> m_fuel_stock;
	
	map<string, float> m_ore_presence_rates;

	bool m_hovered;
	bool m_selected;
};

#endif // STARSHIP_H_INCLUDED
