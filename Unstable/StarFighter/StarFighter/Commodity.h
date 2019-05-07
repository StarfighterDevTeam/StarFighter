#ifndef COMMODITY_H_INCLUDED
#define COMMODITY_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

struct DMS_Coord;

enum CommodityData
{
	Commodity_ID,
	Commodity_Rarity,
	Commodity_Value,
	Commodity_Name,
	Commodity_Description,
	NB_COMMODITY_DATA,
};

enum CommodityRarity
{
	Rarity_Common,
	Rarity_Uncommon,
	Rarity_Collectible,
	Rarity_Epic,
	NB_RARITY,
};

struct Commodity
{
public:
	Commodity(CommodityType type);
	~Commodity();

	Commodity* Clone();
	
	CommodityType m_type;
	CommodityRarity m_rarity;
	int m_value;
};

#endif //COMMODITY_H_INCLUDED