#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include "SpatialObject.h"

enum PlanetType
{
	Planet_Alpha,
	NB_PLANET_TYPES,
};

class Planet : public SpatialObject
{
public :
	Planet(){};
	Planet(PlanetType planet_type, sf::Vector2i sector_index, HostilityLevel hostility);
	virtual ~Planet();

	PlanetType m_planet_type;
};

#endif // PLANET_H_INCLUDED
