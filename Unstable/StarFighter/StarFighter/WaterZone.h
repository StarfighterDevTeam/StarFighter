#ifndef WATERZONE_H_INCLUDED
#define WATERZONE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "WaterTile.h"

class WaterZone
{
public:
	WaterZone(int coord_x, int coord_y);
	~WaterZone();

	int m_coord_x;
	int m_coord_y;
	vector<vector<WaterTile*> > m_watertiles;

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	WaterZone* m_parent;
};

#endif //WATERZONE_H_INCLUDED