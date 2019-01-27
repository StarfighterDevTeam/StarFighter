#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"
#include "Room.h"

#define CREWMEMBER_SIZE							16.f
#define CREWMEMBER_SPEED						70.f
#define CREWMEMBER_SPEED_FLOOD_FACTOR			0.5f
#define CREWMEMBER_HEALTH_MAX					20

#define CREWMEMBER_LIFEBAR_SIZE_X				20.f
#define CREWMEMBER_LIFEBAR_SIZE_Y				4.f
#define CREWMEMBER_LIFEBAR_OFFSET_Y				8.f

class CrewMember : public GameEntity
{
public:
	CrewMember(CrewMemberType type, ShipAlliance alliance);
	~CrewMember();

	CrewMemberType m_type;
	ShipAlliance m_alliance;

	void Update(Time deltaTime);

	RoomTile* GetFreeRoomTile(Room* room);
	void MoveToRoomTile(RoomTile* tile);

	sf::Vector2f m_speed;
	float m_ref_speed;
	int m_health;
	int m_health_max;

	RoomTile* m_tile;
	RoomTile* m_destination;
	vector<RoomTile*> m_current_path;

	float m_repair_timer;
	float m_drowning_timer;
	float m_healing_timer;

	GameEntity* m_lifebar;

private:
	bool FindShortestPath(RoomTile* tileA, RoomTile* tileB);
	void IteratePathFinding(RoomTile* tileA, RoomTile* tileB);
	void IterateEnemyPathFinding(RoomTile* tileA, RoomTile* tileB);
	list<RoomTile*> m_closed_list_pathfind;
	list<RoomTile*> m_open_list_pathfind;
};

#endif //CREWMEMBER_H_INCLUDED