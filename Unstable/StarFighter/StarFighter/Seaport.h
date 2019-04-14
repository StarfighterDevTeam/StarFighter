#ifndef SEAPORT_H_INCLUDED
#define SEAPORT_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"

class Warship;//forward declaration
class Island;

#define RESOURCES_REFRESH_RATE_IN_DAYS			20

enum SeaportType
{
	Seaport_Small,
	NB_SEAPORT_TYPES,
};

class Seaport : public GameEntity
{
public:
	Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType type, Island* island);
	~Seaport();

	int m_coord_x;
	int m_coord_y;
	int m_zone_coord_x;
	int m_zone_coord_y;
	SeaportType m_type;

	WaterTile* m_tile;
	Island* m_island;
	vector<Ship*> m_ships;

	void RemoveShip(Ship* ship);

	//bool CreateIsland(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y);
	
};

#endif //SEAPORT_H_INCLUDED