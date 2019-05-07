#include "Commodity.h"

extern Game* CurrentGame;

Commodity::Commodity(CommodityType type, int quantity)
{
	m_quantity = quantity;
	m_type = type;

	m_rarity = Rarity_Common;
	m_value = 1;
}

Commodity::~Commodity()
{
	
}