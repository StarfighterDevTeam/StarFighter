#ifndef ORE_H_INCLUDED
#define ORE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

enum OreData
{
	OreData_Name,
	OreData_TextureName,
	OreData_Weight,
	OreData_Extraction,
	OreData_Price,
	OreData_Propulsion,
	OreData_PropulsionSpeedBonus,
};

class Ore : public GameObject
{
public :
	Ore() {};
	Ore(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Ore();
	void update(sf::Time deltaTime) override;
	static Ore* CreateOre(string name);

	string m_display_name;
	size_t m_weight;
	size_t m_price;
	size_t m_propulsion;
	float m_extraction_duration;
	float m_propulsion_speed_bonus;
};

#endif // ORE_H_INCLUDED
