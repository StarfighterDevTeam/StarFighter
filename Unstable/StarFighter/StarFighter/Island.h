#ifndef ISLAND_H_INCLUDED
#define ISLAND_H_INCLUDED

#include "Game.h"

#include "WaterZone.h"
#include "Seaport.h"

class Warship;//forward declaration

class Island : public GameEntity
{
public:
	Island(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y);
	~Island();

	int m_upcorner_x;
	int m_upcorner_y;
	int m_width;
	int m_height;

	vector<WaterTile*> m_tiles;

	Seaport* AddSeaport(SeaportType type);
	Seaport* m_seaport;

	//bool CreateIsland(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y);
	
};

#endif //ISLAND_H_INCLUDED