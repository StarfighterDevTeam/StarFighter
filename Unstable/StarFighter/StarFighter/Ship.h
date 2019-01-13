#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "CrewMember.h"
#include "WaterZone.h"
#include "Island.h"

#define SHIP_TEXTURE_NAME				"2D/enemy_icon.png"
#define SHIP_TACTICAL_TEXTURE_NAME		"2D/enemy_icon_tactical.png"
#define WARSHIP_TEXTURE_NAME			"2D/warship_icon.png"
#define WARSHIP_TACTICAL_TEXTURE_NAME	"2D/warship_icon_tactical.png"

#define	CRUISE_SPEED					30.f
#define ANGLE_SPEED						30.f
#define	TACTICAL_SPEED_FACTOR			0.03f

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
	void Draw(sf::RenderTexture& screen);

	void UpdatePositionOnMap(DMS_Coord warship_DMS);
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
	vector<WaterTile*> m_current_path;
	Seaport* m_seaport;

	static bool IsConnectedToRoomTile(RoomTile* tileA, RoomTile* tileB);
	bool IsConnectedToRoom(Room* room);
	Room* AddRoom(int upcorner_x, int upcorner_y, int width, int height, RoomType type);
	bool AddConnexion(int tileA_x, int tileA_y, int tileB_x, int tileB_y);
	Room* ConnectRooms();
	CrewMember* AddCrewMember(CrewMember* crew, Room* room);
	Weapon* AddWeapon(Weapon* weapon, Room* room, Ship* ship);
	bool FireWeapon(Weapon* weapon, Time deltaTime);
	static WaterTile* GetWaterTileAtDMSCoord(DMS_Coord coord);

	//tactical scale
	float m_tactical_posx;
	float m_tactical_posy;
	GameEntity* m_tactical_icon;//link between tactical and strategic representation of the ship
	
	void UpdateTacticalPositionOnMap();
};

#endif //SHIP_H_INCLUDED