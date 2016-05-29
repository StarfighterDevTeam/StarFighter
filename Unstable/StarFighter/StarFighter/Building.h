#ifndef BUILDING_H_INCLUDED
#define BUILDING_H_INCLUDED

#include "Ore.h"

enum BuildingData
{
	BuildingData_Name,
	BuildingData_TextureName,
	BuildingData_Slots,
	BuildingData_Stock,
	BuildingData_CanExtractOre,
	BuildingData_ExtractionDurationBonus,
};

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
};

#endif // BUILDING_H_INCLUDED
