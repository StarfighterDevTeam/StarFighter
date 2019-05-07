#include "Commodity.h"

extern Game* CurrentGame;

Commodity::Commodity(CommodityType type)
{
	m_type = type;

	m_rarity = Rarity_Common;
	m_value = 1;
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