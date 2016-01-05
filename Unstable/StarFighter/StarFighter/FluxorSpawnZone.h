#ifndef FLUXORSPAWNZONE_H_INCLUDED
#define FLUXORSPAWNZONE_H_INCLUDED

#include "Fluxor.h"

class FluxorSpawnZone
{
public :
	FluxorSpawnZone();
	FluxorSpawnZone(sf::FloatRect spawn_bounds, unsigned int population);
	virtual ~FluxorSpawnZone();

	sf::FloatRect m_spawn_bounds;
	unsigned int m_population;
};

#endif // FLUXORSPAWNZONE_H_INCLUDED
