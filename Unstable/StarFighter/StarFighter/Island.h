#ifndef ISLAND_H_INCLUDED
#define ISLAND_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"

class Warship;//forward declaration

class Island : public WaterTile
{
public:
	Island(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y);
	~Island();

	vector<WaterTile*> m_tiles;

	//bool CreateIsland(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y);
	
};

#endif //ISLAND_H_INCLUDED