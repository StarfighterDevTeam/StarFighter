#ifndef WATERTILE_H_INCLUDED
#define WATERTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Warship;//forward declaration
class WaterZone;
class Island;
class Seaport;

struct DMS_Coord
{
	DMS_Coord(){};
	DMS_Coord(int degree_x, int minute_x, float second_x, int degree_y, int minute_y, float second_y)
	{
		SetCoord(degree_x, minute_x, second_x, degree_y, minute_y, second_y);
	}

	int m_degree_x;
	int m_minute_x;
	float m_second_x;

	int m_degree_y;
	int m_minute_y;
	float m_second_y;

	void SetCoord(int degree_x, int minute_x, float second_x, int degree_y, int minute_y, float second_y){
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
	Water_Empty,//below this = navigable
	Water_Island,
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
	bool m_can_be_seen;

	void UpdatePosition(DMS_Coord warship_DMS);
	static DMS_Coord GetDMSCoord(sf::Vector2f position, DMS_Coord warship_DMS);
	static bool SameDMS(DMS_Coord DMS_a, DMS_Coord DMS_b);

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	WaterTile* m_parent;

	Island* m_island;
	Seaport* m_seaport;

	int m_cost;

	bool m_is_wreck_location;
};

#endif //WATERTILE_H_INCLUDED