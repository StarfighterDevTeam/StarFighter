#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Weapon.h"
#include "Engine.h"
#include "Rudder.h"
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
	vector<CrewMember*> m_crew[2];
	vector<Weapon*> m_weapons;
	vector<vector<RoomTile*> > m_tiles;
	vector<Engine*> m_engines;
	Rudder* m_rudder;
	vector<RoomTile*> m_prison_cells;

	int m_nb_crew;
	int m_nb_crew_max;
	int m_health_max;
	int m_health;
	int m_flood;
	int m_flood_max;
	int m_lifeboats;

	float m_flee_timer;
	float m_flee_count;
	bool m_is_charging_flee_count;
	bool m_is_fleeing;

	float m_sinking_timer;

	void UpdateFleeing(Time deltaTime);
	void UpdateSinking(Time deltaTime);
	void ApplyAlphaToShip(Uint8 alpha);

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
	CrewMember* AddCrewMember(CrewMember* crew, Room* room = NULL);

	void AddWeaponToTile(Weapon* weapon, RoomTile* tile);
	void AddEngineToTile(RoomTile* tile);
	void AddRudderToTile(RoomTile* tile);

	bool FireWeapon(Weapon* weapon, Time deltaTime, Ship* target);
	static WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);
	bool IsFlooded();

	bool ImprisonCrew(CrewMember* crew);
	bool IsPrisonCellFree();
	void UpdatePrisonerEscape(CrewMember* crew, sf::Time deltaTime);
	void EscapeToRandomTileInRoom(CrewMember* crew, Room* room);
	void EscapeToRandomRoom(CrewMember* crew);

	void BuildShip();
	void CenterRoomPositions(bool is_enemy);
	void Reset();

	void UpdateFlooding(Time deltaTime);
	void UpdateShipOffset();

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