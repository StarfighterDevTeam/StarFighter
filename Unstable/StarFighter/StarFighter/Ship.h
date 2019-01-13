#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

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

	vector<Room*> m_rooms;
	vector<RoomConnexion*> m_connexions;
	vector<CrewMember*> m_crew;
	vector<Weapon*> m_weapons;
	
	//int m_nb_crew[NB_CREW_TYPES];
	//int m_nb_crew_working[NB_CREW_TYPES];
	//int m_nb_crew_max;

	WaterTile* m_tile;
	WaterTile* m_destination;
	vector<WaterTile*> m_tiles_can_be_seen;
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;
	bool m_is_minimized;

	static bool IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB);
	bool IsConnectedToRoom(Room* room);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type, bool minimized = false);
	Room* AddRoomMinimized(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	Weapon* AddWeapon(Weapon* weapon, Room* room, Ship* ship);
	bool FireWeapon(Weapon* weapon, Time deltaTime);
	static WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);

	void BuildShip(bool minimized);
};

#endif //SHIP_H_INCLUDED