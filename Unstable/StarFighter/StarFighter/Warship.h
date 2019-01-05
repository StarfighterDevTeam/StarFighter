#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Island.h"

#define	CRUISE_SPEED				10

class Warship : public GameEntity
{
public:
	Warship(DMS_Coord coord);
	~Warship();

	vector<Room*> m_rooms;
	vector<RoomConnexion*> m_connexions;
	vector<CrewMember*> m_crew;

	WaterZone* m_zone;
	WaterTile* m_tile;
	WaterTile* m_destination;
	Seaport* m_seaport;

	DMS_Coord m_DMS;
	float m_angle;
	sf::Vector2f m_speed;

	bool SetDMSCoord(DMS_Coord coord);
	DMS_Coord GetDMSCoord(sf::Vector2f position);
	WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);
	bool CanViewWaterTile(WaterTile* tile);
	int GetDistanceToWaterTile(WaterTile* tile);
	float GetDistanceFloatToWaterTile(WaterTile* tile);
	void UpdateRotation();
	static float GetAngleForVector(sf::Vector2f vector);
	bool SetSailsToWaterTile(WaterTile* tile);
	bool CheckPathfindToWaterTile(WaterTile* tile);
	bool IsPositionCollidingWithIsland(sf::Vector2f position);
	
	void Update(Time deltaTime);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	void UpdateCrewMembersCountPerRoom(Room* room);
};

#endif //WARSHIP_H_INCLUDED