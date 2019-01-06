#ifndef SEAPORT_H_INCLUDED
#define SEAPORT_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"

class Warship;//forward declaration
class Island;

enum SeaportType
{
	Seaport_Small,
};

class Seaport : public GameEntity
{
public:
	Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType type, Island* island);
	~Seaport();

	int m_coord_x;
	int m_coord_y;
	SeaportType m_type;

	WaterTile* m_tile;
	Island* m_island;
	vector<Warship*> m_ships;

	void RemoveShip(Warship* ship);

	//bool CreateIsland(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y);
	
};

#endif //SEAPORT_H_INCLUDED