#ifndef BEACON_H_INCLUDED
#define BEACON_H_INCLUDED

#include "EscortShip.h"

class Beacon : public SpatialObject
{
public:
	Beacon(sf::Vector2i sector_index);
	~Beacon();

	vector<AIShip*> m_ships_to_create;
};

#endif // BEACON_H_INCLUDED
