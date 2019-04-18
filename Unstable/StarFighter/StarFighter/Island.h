#ifndef ISLAND_H_INCLUDED
#define ISLAND_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"

class Warship;//forward declaration
class Seaport;

class Island : public GameEntity
{
public:
	Island(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y, Seaport* seaport);
	~Island();

	int m_upcorner_x;
	int m_upcorner_y;
	int m_width;
	int m_height;
	int m_zone_coord_x;
	int m_zone_coord_y;

	vector<WaterTile*> m_tiles;
	WaterTile* m_tile;
	Seaport* m_seaport;
};

#endif //ISLAND_H_INCLUDED