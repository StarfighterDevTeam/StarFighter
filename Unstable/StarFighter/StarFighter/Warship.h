#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "WaterZone.h"

class Warship : public GameEntity
{
public:
	Warship();
	~Warship();

	vector<Room*> m_rooms;
	vector<RoomConnexion*> m_connexions;
	vector<CrewMember*> m_crew;

	WaterZone* m_zone;
	WaterTile* m_tile;
	
	DMS_Coord m_DMS;
	float m_angle;

	bool SetDMSCoord(DMS_Coord coord);
	WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);
	bool IsWaterTileInViewRange(WaterTile* tile);

	void InitWarship();
	void Update(Time deltaTime);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	void UpdateCrewMembersCountPerRoom(Room* room);
};

#endif //WARSHIP_H_INCLUDED