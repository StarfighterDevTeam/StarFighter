#include "Warship.h"

extern Game* CurrentGame;

Warship::Warship()
{
	
}

Warship::~Warship()
{
	
}

void Warship::InitWarship()
{
	//left
	AddRoom(1, 3, 3, 4, Room_Weapon);
	AddRoom(0, 7, 4, 6, Room_Gold);
	AddRoom(0, 13, 4, 6, Room_Relic);
	AddRoom(0, 19, 4, 3, Room_Sword);

	//mid
	AddRoom(6, 0, 4, 3, Room_Weapon);
	AddRoom(4, 3, 8, 6, Room_Navigation);
	AddRoom(4, 9, 8, 10, Room_Crewquarter);
	AddRoom(4, 19, 8, 3, Room_Ammo);
	AddRoom(5, 22, 6, 3, Room_Engine);

	//right
	AddRoom(12, 3, 3, 4, Room_Weapon);
	AddRoom(12, 7, 4, 6, Room_Fishing);
	Room* room2 = AddRoom(12, 13, 4, 6, Room_Kitchen);
	Room* room = AddRoom(12, 19, 4, 3, Room_Lifeboat);

	//crew
	CrewMember* crew = new CrewMember(Crew_Pirate);
	AddCrewMember(crew, room);

	CrewMember* crew2 = new CrewMember(Crew_Civilian);
	AddCrewMember(crew2, room2);
}

Room* Warship::AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type)
{
	Room* room = new Room(upcorner_x, upcorner_y, width, height, type);
	m_rooms.push_back(room);

	return room;
}

CrewMember* Warship::AddCrewMember(CrewMember* crew, Room* room)
{
	if (room == NULL)
	{
		return NULL;
	}

	RoomTile* tile = room->GetFreeRoomTile();

	if (tile == NULL)
	{
		return NULL;
	}

	crew->m_position = tile->m_position;

	m_crew.push_back(crew);
	room->m_crew.push_back(crew);

	//UI
	crew->m_shape_container.setPosition(crew->m_position);
	crew->m_text.SetPosition(crew->m_position);

	return crew;
}