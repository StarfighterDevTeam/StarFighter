#ifndef ROOMTILE_H_INCLUDED
#define ROOMTILE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"

class Room;//forward declaration
class CrewMember;


#define CONNEXION_WIDTH			4.f
#define CONNEXION_OPEN_LENGTH	4.f		

class RoomConnexion : public GameEntity
{
public:
	RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool open, bool minimized);
	
	pair<RoomTile*, RoomTile*> m_tiles;
	bool m_open;
	bool m_locked;

	void Update(Time deltaTime);
	void SetLock(bool locked);
};

#define ROOMTILE_SIZE				32.f
#define ROOMTILE_OFFSET_X			400.f
#define ROOMTILE_OFFSET_Y			500.f

#define ROOMTILE_MINI_SIZE			24.f
#define ROOMTILE_MINI_OFFSET_X		1550.f
#define ROOMTILE_MINI_OFFSET_Y		500.f

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

	static RoomTile* GetRoomTileAtCoord(int coord_x, int coord_y, bool minimized);
	bool IsConnectedToRoomTile(RoomTile* tile);

	//pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	RoomTile* m_parent;
	Weapon* m_weapon;
};

#endif //ROOMTILE_H_INCLUDED