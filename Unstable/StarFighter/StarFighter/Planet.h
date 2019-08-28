#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include "Marker.h"

#define NB_PLANET_TYPES		18

class Planet : public SpatialObject
{
public :
	Planet(){};
	Planet(int planet_type, sf::Vector2i sector_index, HostilityLevel hostility);
	virtual ~Planet();
	void SetHostility(HostilityLevel hostility) override;
};

#endif // PLANET_H_INCLUDED
