#ifndef SEAPORT_H_INCLUDED
#define SEAPORT_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"
#include "Island.h"
#include "Location.h"

class Warship;//forward declaration

enum SeaportType
{
	Seaport_Small,
	Seaport_Medium,
	Seaport_Large,
	NB_SEAPORT_TYPES,
};

class Seaport : public Location
{
public:
	Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType seaport_type);
	~Seaport();

	SeaportType m_seaport_type;
	Island* m_island;
	vector<Ship*> m_ships;
	void RemoveShip(Ship* ship);
	Island* AddIsland(int upcorner_x, int upcorner_y, int width, int height);
};

#endif //SEAPORT_H_INCLUDED