#include "WaterZone.h"

extern Game* CurrentGame;

WaterZone::WaterZone(int coord_x, int coord_y)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;

	//pathfinding
	m_heuristic = 0;
	m_movement_cost = 0;
	m_G_value = 0;
	m_parent = NULL;

	//water tiles
	for (int y = 0; y < NB_WATER_TILE_Y; y++)
	{
		vector<WaterTile*> vec;
		for (int x = 0; x < NB_WATER_TILE_X; x++)
		{
			WaterTile* tile = new WaterTile(x, y, Water_Empty, this, m_coord_x, m_coord_y);
			vec.push_back(tile);
		}

		m_watertiles.push_back(vec);
	}
}

WaterZone::~WaterZone()
{
	for (vector<vector<WaterTile*> >::iterator it = m_watertiles.begin(); it != m_watertiles.end(); it++)
	{
		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			delete *it2;
		}
	}
}