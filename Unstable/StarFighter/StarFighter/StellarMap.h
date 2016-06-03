#ifndef STELLARMAP_H_INCLUDED
#define STELLARMAP_H_INCLUDED

#include "StockEntity.h"

class StellarZone : public sf::RectangleShape
{
public:
	StellarZone();
	virtual ~StellarZone();

	bool m_known;
};

class StellarMap
{
public :
	StellarMap();
	virtual ~StellarMap();
	void update(sf::Time deltaTime);

	StellarZone* m_zones[STELLAR_ZONES_PER_LINE][STELLAR_ZONES_PER_LINE];
};

#endif // STELLARMAP_H_INCLUDED
