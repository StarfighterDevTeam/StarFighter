#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Island.h"

#define	CRUISE_SPEED				40

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
	vector<WaterTile*> m_tiles_can_be_seen;
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;
	float m_pathfind_cooldown_timer;

	DMS_Coord m_DMS;
	float m_angle;
	sf::Vector2f m_speed;

	bool SetDMSCoord(DMS_Coord coord);
	DMS_Coord GetDMSCoord(sf::Vector2f position);
	float GetDistanceSquaredInSecondsDMS(WaterTile* tile);
	WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);
	bool CanViewWaterTile(WaterTile* tile);
	int GetDistanceToWaterTile(WaterTile* tile);
	float GetDistanceFloatToWaterTile(WaterTile* tile);
	void UpdateRotation();
	bool SetSailsToWaterTile(WaterTile* tile);

	void Update(Time deltaTime);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	void UpdateCrewMembersCountPerRoom(Room* room);

private:
	void FindShortestPath(WaterTile* tileA, WaterTile* tileB);
	void IteratePathFindingOnIndex(WaterTile* tileA, WaterTile* tileB);
	list<WaterTile*> m_closed_list_pathfind;
	list<WaterTile*> m_open_list_pathfind;
};

#endif //WARSHIP_H_INCLUDED