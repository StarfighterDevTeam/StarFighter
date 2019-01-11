#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Island.h"
#include "Compass.h"
#include "Ship.h"

#define	CRUISE_SPEED					60.f
#define ANGLE_SPEED						30.f

#define COMPASS_MODE					false

class Warship : public Ship
{
public:
	Warship(DMS_Coord coord);
	~Warship();

	vector<Room*> m_rooms;
	vector<RoomConnexion*> m_connexions;
	vector<CrewMember*> m_crew;

	float m_desired_angle;
	float m_angle_speed;
	struct Compass m_compass;

	DMS_Coord GetDMSCoord(sf::Vector2f position);
	float GetDistanceSquaredInSecondsDMS(WaterTile* tile);
	bool CanViewWaterTile(WaterTile* tile);
	int GetDistanceToWaterTile(WaterTile* tile);
	float GetDistanceFloatToWaterTile(WaterTile* tile);
	bool SetSailsToWaterTile(WaterTile* tile);
	bool IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB);

	void Update(Time deltaTime);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	void UpdateCrewMembersCountPerRoom(Room* room);

private:
	void FindShortestPath(WaterTile* tileA, WaterTile* tileB);
	void IteratePathFinding(WaterTile* tileA, WaterTile* tileB);
	list<WaterTile*> m_closed_list_pathfind;
	list<WaterTile*> m_open_list_pathfind;

	bool RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB);
	bool RayTracingContainsIslandForPathfind(WaterTile* tileA, WaterTile* tileB);
	void RayTracingGetPath(WaterTile* tileA, WaterTile* tileB);
};

#endif //WARSHIP_H_INCLUDED