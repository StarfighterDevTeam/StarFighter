#ifndef CREWMEMBER_H_INCLUDED
#define CREWMEMBER_H_INCLUDED

#include "Game.h"

#include "Room.h"

#define CREWMEMBER_SIZE		16.f
#define CREWMEMBER_SPEED	70.f

enum CrewMemberType
{
	Crew_Unknown,
	Crew_Pirate,
	Crew_Civilian,
	Crew_Slave,
	Crew_Mecha,
	NB_CREW_TYPES,
};

class CrewMember : public GameEntity
{
public:
	CrewMember(CrewMemberType type);
	~CrewMember();

	CrewMemberType m_type;

	void Update(Time deltaTime);
	bool MoveToRoom(Room* room);
	void MoveToRoomTile(RoomTile* tile);

	sf::Vector2f m_speed;
	float m_speed_max;

	RoomTile* m_tile;
	vector<RoomTile*> m_current_path;

private:
	void FindShortestPath(RoomTile* tileA, RoomTile* tileB);
	void IteratePathFindingOnIndex(RoomTile* tileA, RoomTile* tileB);
	list<RoomTile*> m_closed_list_pathfind;
	list<RoomTile*> m_open_list_pathfind;
};

#endif //CREWMEMBER_H_INCLUDED