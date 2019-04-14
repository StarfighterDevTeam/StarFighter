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
	int m_zone_coord_x;
	int m_zone_coord_y;

	vector<WaterTile*> m_tiles;

	Seaport* AddSeaport(SeaportType type);
	Seaport* AddSeaport(SeaportType type, int coord_x, int coord_y);
	Seaport* m_seaport;
	WaterTile* m_tile;

	int m_choicesID[4];
	string m_display_name;
	int m_visited_countdown;
};

#endif //ISLAND_H_INCLUDED