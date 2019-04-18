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
	NB_SEAPORT_TYPES,
};

class Seaport : public Location
{
public:
	Seaport(int coord_x, int coord_y, int zone_coord_x, int zone_coord_y, SeaportType type);
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

	string m_display_name;
	int m_visited_countdown;
	int m_choicesID[NB_CHOICES_MAX];

	Island* AddIsland(int upcorner_x, int upcorner_y, int width, int height);
};

#endif //SEAPORT_H_INCLUDED