#ifndef ROOMTILE_H_INCLUDED
#define ROOMTILE_H_INCLUDED

#include "SFTextPop.h"
#include "Game.h"
#include "GameEntity.h"

class Room;//forward declaration
class CrewMember;
class RoomConnexion;
class Engine;
class Rudder;

enum Hull
{
	Hull_Left,
	Hull_Up,
	Hull_Right,
	Hull_Down,
	Hull_None,
};

class RoomTile : public GameEntity
{
public:
	RoomTile(int coord_x, int coord_y, Room* room, float size);
	~RoomTile();

	Room* m_room;
	CrewMember* m_crew;
	int m_coord_x;
	int m_coord_y;
	RoomConnexion* m_connexion;
	Hull m_hull;
	bool m_is_pierced;
	int m_flood;
	bool m_flood_dir[4];
	float m_flooding_timer;
	float m_evaporation_timer;
	int m_health;
	int m_health_max;

	static RoomTile* GetRoomTileAtCoord(int coord_x, int coord_y, bool is_enemy);

	//systems
	ShipSystem m_system;
	RoomTile* m_system_tile;
	RoomTile* m_operator_tile;
	Weapon* m_weapon;
	Engine* m_engine;
	Rudder* m_rudder;
	bool m_is_prison;

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	RoomTile* m_parent;
};

#endif //ROOMTILE_H_INCLUDED