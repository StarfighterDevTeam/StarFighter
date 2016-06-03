#ifndef STELLARMAP_H_INCLUDED
#define STELLARMAP_H_INCLUDED

//#include "StockEntity.h"
#include "Game.h"
#include "Location.h"

//class StellarZone : public sf::RectangleShape
//{
//public:
//	StellarZone();
//	virtual ~StellarZone();
//};

class StellarMap : public StellarMapVirtual
{
public :
	StellarMap();
	virtual ~StellarMap();
	void update(sf::Time deltaTime) override;

	bool GenerateRandomZoneContent(sf::Vector2u zone_index) override;
	//void ExpandKnownStellarMap(sf::Vector2u zone_index) override;

	//StellarZone* m_zones[STELLAR_ZONES_PER_LINE][STELLAR_ZONES_PER_LINE];
};

#endif // STELLARMAP_H_INCLUDED
