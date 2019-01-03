#ifndef WATERTILE_H_INCLUDED
#define WATERTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Warship;//forward declaration
class WaterZone;

#define WATERTILE_SIZE			64.f
#define WATERTILE_OFFSET_X		200.f
#define WATERTILE_OFFSET_Y		100.f
#define NB_WATER_TILE_X			60
#define NB_WATER_TILE_Y			60

struct DMS_Coord
{
	int m_degree_x;
	int m_minute_x;
	int m_second_x;

	int m_degree_y;
	int m_minute_y;
	int m_second_y;

	void SetCoord(int degree_x, int minute_x, int second_x, int degree_y, int minute_y, int second_y){
		m_degree_x = degree_x;
		m_minute_x = minute_x;
		m_second_x = second_x;
		m_degree_y = degree_y;
		m_minute_y = minute_y;
		m_second_y = second_y;
	}
};

enum WaterTileType
{
	Water_Empty,
};

class WaterTile : public GameEntity
{
public:
	WaterTile(int coord_x, int coord_y, WaterTileType type, WaterZone* zone, int zone_coord_x, int zone_coord_y, float size = WATERTILE_SIZE);
	~WaterTile();

	int m_coord_x;
	int m_coord_y;
	WaterTileType m_type;
	WaterZone* m_zone;
	DMS_Coord m_DMS;//degree/min/sec

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	WaterTile* m_parent;
};

#endif //WATERTILE_H_INCLUDED