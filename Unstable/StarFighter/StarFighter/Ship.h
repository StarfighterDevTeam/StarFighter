#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Weapon.h"
#include "CrewMember.h"
#include "WaterZone.h"
#include "Island.h"

class Ship : public GameEntity
{
public:
	Ship(DMS_Coord coord, ShipType type, ShipAlliance alliance, string display_name);
	~Ship();

	ShipType m_type;
	ShipAlliance m_alliance;
	string m_display_name;

	DMS_Coord m_DMS;//degree/min/sec
	sf::Vector2f m_speed;
	float m_angle;
	bool m_can_be_seen;

	void Update(Time deltaTime, DMS_Coord warship_DMS, bool tactical_combat);
	void UpdatePosition(DMS_Coord warship_DMS);
	bool SetDMSCoord(DMS_Coord coord);

	void UpdateAnimation();
	void GetAngleForSpeed(float& angle);

	vector<Room*> m_rooms;
	vector<RoomConnexion*> m_connexions;
	vector<CrewMember*> m_crew;
	vector<Weapon*> m_weapons;
	vector<vector<RoomTile*> > m_tiles;

	vector<RoomTile*> m_systems[NB_SHIP_SYSTEMS];
	
	//int m_nb_crew[NB_CREW_TYPES];
	//int m_nb_crew_working[NB_CREW_TYPES];
	int m_nb_crew;
	int m_nb_crew_max;
	int m_health_max;
	int m_health;
	int m_flood;
	int m_flood_max;

	float m_flee_timer;
	float m_flee_count;
	bool m_is_fleeing;
	void UpdateFleeingBar(Time deltaTime);

	WaterTile* m_tile;
	WaterTile* m_destination;
	vector<WaterTile*> m_tiles_can_be_seen;
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;

	float m_distance_combat;
	sf::Vector2i m_rooms_size;
	string m_textureName;

	static bool IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB);
	bool IsConnectedToRoom(Room* room);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	void FlagHullRoomTiles();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	void AddWeaponToTile(Weapon* weapon, RoomTile* tile);
	void AddSystemToTile(ShipSystem system, RoomTile* tile);
	bool FireWeapon(Weapon* weapon, Time deltaTime, Ship* target);
	static WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);

	void BuildShip();
	void CenterRoomPositions(bool is_enemy);

	void UpdateFlooding(Time deltaTime);

	void InitCombat();
	void RestoreHealth();
	void RestoreWeaponsHealth();

	//systems
	static bool IsSystemOperational(ShipSystem system, RoomTile* tile);
	float GetDodgeChances();

private:
	int m_rooms_min_upcorner_x;
	int m_rooms_min_upcorner_y;
	int m_rooms_max_offset_x;
	int m_rooms_max_offset_y;
};

#endif //SHIP_H_INCLUDED