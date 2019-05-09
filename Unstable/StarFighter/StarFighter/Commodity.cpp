#include "Commodity.h"

extern Game* CurrentGame;

Commodity::Commodity(CommodityType type) : GameEntity(UI_Commodity)
{
	m_type = type;
	int typeID = (int)(type) - 1;//ID 1 starts at line 0 in the DB
	m_rarity = (CommodityRarity)stoi((*CurrentGame).m_commodities_config[typeID][Commodity_Rarity]);
	m_value = stoi((*CurrentGame).m_commodities_config[typeID][Commodity_Value]);
	m_display_name = StringReplace((*CurrentGame).m_commodities_config[typeID][Commodity_Name], "_", " ");
	m_description = StringReplace((*CurrentGame).m_commodities_config[typeID][Commodity_Name], "_", " ");
	m_texture_name = (*CurrentGame).m_commodities_config[typeID][Commodity_Texture];
}

Commodity::~Commodity()
{
	
}