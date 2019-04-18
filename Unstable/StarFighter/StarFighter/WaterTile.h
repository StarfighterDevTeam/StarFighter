#ifndef WATERTILE_H_INCLUDED
#define WATERTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Location.h"

class Warship;//forward declaration
class WaterZone;
class Island;
class Seaport;

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

	Location* m_location;

	int m_cost;
};

#endif //WATERTILE_H_INCLUDED