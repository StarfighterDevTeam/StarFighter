#include "Commodity.h"

extern Game* CurrentGame;

Commodity::Commodity(CommodityType type)
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

Commodity* Commodity::Clone()
{
	Commodity* clone = new Commodity(m_type);
	clone->m_rarity = m_rarity;
	clone->m_value = m_value;

	return clone;
}