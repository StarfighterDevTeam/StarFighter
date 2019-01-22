#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"
#include "Weapon.h"
#include "Room.h"

#define CREWMEMBER_SIZE							16.f
#define CREWMEMBER_SPEED						70.f

class CrewMember : public GameEntity
{
public:
	CrewMember(CrewMemberType type);
	~CrewMember();

	CrewMemberType m_type;

	void Update(Time deltaTime);

	RoomTile* GetFreeRoomTile(Room* room);
	bool MoveToRoom(Room* room);
	void MoveToRoomTile(RoomTile* tile);

	sf::Vector2f m_speed;
	float m_ref_speed;
	int m_health;
	int m_health_max;

	RoomTile* m_tile;
	RoomTile* m_destination;
	vector<RoomTile*> m_current_path;

	float m_repair_timer;

private:
	bool FindShortestPath(RoomTile* tileA, RoomTile* tileB);
	void IteratePathFinding(RoomTile* tileA, RoomTile* tileB);
	list<RoomTile*> m_closed_list_pathfind;
	list<RoomTile*> m_open_list_pathfind;
};

#endif //CREWMEMBER_H_INCLUDED