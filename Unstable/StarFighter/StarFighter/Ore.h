#ifndef ORE_H_INCLUDED
#define ORE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

enum OreType
{
	OreType_Iron,
	OreType_Silver,
	OreType_Gold,
	NBVAL_OreTypes,
};

enum OreData
{
	OreData_Name,
	OreData_Weight,
	OreData_Extraction,
	OreData_Price,
};

class Ore : public GameObject
{
public :
	Ore() {};
	Ore(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Ore();
	void update(sf::Time deltaTime) override;
	static Ore* CreateOre(OreType type);

	string m_display_name;
	OreType m_type;
	size_t m_weight;
	size_t m_price;
	float m_extraction_duration;
};

#endif // ORE_H_INCLUDED
