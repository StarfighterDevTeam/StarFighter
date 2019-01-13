#ifndef ROOMTILE_H_INCLUDED
#define ROOMTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Room;//forward declaration
class CrewMember;
class RoomConnexion;

enum Hull
{
	Hull_None,
	Hull_Left,
	Hull_Right,
	Hull_Up,
	Hull_Down,
};

class RoomTile : public GameEntity
{
public:
	RoomTile(int coord_x, int coord_y, Room* room, float size, bool minimized);
	~RoomTile();

	Room* m_room;
	CrewMember* m_crew;
	int m_coord_x;
	int m_coord_y;
	RoomConnexion* m_connexion;
	Hull m_hull;
	bool m_pierced;

	static RoomTile* GetRoomTileAtCoord(int coord_x, int coord_y, bool minimized);
	bool IsConnectedToRoomTile(RoomTile* tile);

	bool Pierce();

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	RoomTile* m_parent;
	Weapon* m_weapon;
};

#endif //ROOMTILE_H_INCLUDED