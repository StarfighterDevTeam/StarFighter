#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "WaterZone.h"
#include "Island.h"

enum ShipType
{
	Ship_Goellete,
	Ship_Warship,
};

class Ship : public GameEntity
{
public:
	Ship(DMS_Coord coord, ShipType type);
	~Ship();

	ShipType m_type;

	DMS_Coord m_DMS;//degree/min/sec
	sf::Vector2f m_speed;
	float m_angle;
	bool m_can_be_seen;

	void Update(Time deltaTime, DMS_Coord warship_DMS);
	void UpdatePosition(DMS_Coord warship_DMS);
	bool SetDMSCoord(DMS_Coord coord);

	void UpdateAnimation();
	void GetAngleForSpeed(float& angle);

	//vector<Room*> m_rooms;
	//vector<RoomConnexion*> m_connexions;
	//vector<CrewMember*> m_crew;
	
	//int m_nb_crew[NB_CREW_TYPES];
	//int m_nb_crew_working[NB_CREW_TYPES];
	//int m_nb_crew_max;

	WaterTile* m_tile;
	WaterTile* m_destination;
	vector<WaterTile*> m_tiles_can_be_seen;
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;	

	static bool IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB);
	bool IsConnectedToRoom(Room* room);

	static WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);
};

#endif //SHIP_H_INCLUDED