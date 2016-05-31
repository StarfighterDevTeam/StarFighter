#ifndef BUILDING_H_INCLUDED
#define BUILDING_H_INCLUDED

#include "Ore.h"

class Building : public GameObject
{
public :
	Building() {};
	Building(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Building();
	void update(sf::Time deltaTime) override;

	static Building* CreateBuilding(string name);

	bool Extract(Ore* ore);

	string m_display_name;
	size_t m_stock;
	size_t m_stock_max;
	size_t m_slots;
	bool m_can_extract_ore;
	sf::Clock m_extraction_clock;
	float m_extraction_duration_bonus;
	Ore* m_current_extraction;

	map<string, size_t> m_cost;
	map<string, size_t> m_starship_productions;
};

#endif // BUILDING_H_INCLUDED
