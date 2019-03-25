#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Weapon.h"
#include "Engine.h"
#include "Rudder.h"
#include "CrewMember.h"
#include "WaterZone.h"
#include "Island.h"
#include "CombatInterface.h"

#define	CRUISE_SPEED					50.f
#define ANGLE_SPEED						30.f

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

	void UpdatePosition(DMS_Coord warship_DMS);
	void UpdateTactical(Time deltaTime);
	void UpdateStrategical(Time deltaTime);

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

	struct CombatInterface m_combat_interface;

	void UpdateFleeing(Time deltaTime);
	void UpdateSinking(Time deltaTime);
	void ApplyAlphaToShip(Uint8 alpha);

	WaterTile* m_tile;
	WaterTile* m_destination;
	vector<WaterTile*> m_tiles_can_be_seen;
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;
	int m_resources[NB_RESOURCES_TYPES];

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

	//navigation
	float GetDistanceSquaredInSecondsDMS(WaterTile* tile);
	bool CanViewWaterTile(WaterTile* tile);
	int GetDistanceToWaterTile(WaterTile* tile);
	float GetDistanceFloatToWaterTile(WaterTile* tile);
	bool SetSailsToWaterTile(WaterTile* tile);
	bool IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB);

private:
	int m_rooms_min_upcorner_x;
	int m_rooms_min_upcorner_y;
	int m_rooms_max_offset_x;
	int m_rooms_max_offset_y;

	void FindShortestPath(WaterTile* tileA, WaterTile* tileB);
	void IteratePathFinding(WaterTile* tileA, WaterTile* tileB);
	list<WaterTile*> m_closed_list_pathfind;
	list<WaterTile*> m_open_list_pathfind;

	bool RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB);
	bool RayTracingContainsIslandForPathfind(WaterTile* tileA, WaterTile* tileB);
	void RayTracingGetPath(WaterTile* tileA, WaterTile* tileB);
};

#endif //SHIP_H_INCLUDED