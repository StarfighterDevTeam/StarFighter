#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"
#include "Ammo.h"
#include "Room.h"
#include "CrewMember.h"
#include "WaterZone.h"

class Ship;//forward declaration

#define CANNON_HEALTH_MAX		30
#define CANNON_RATE_OF_FIRE		2.f

enum WeaponType
{
	Weapon_Cannon,
};

class Weapon : public GameEntity
{
public:
	Weapon(WeaponType type, bool is_enemy);
	~Weapon();

	WeaponType m_type;
	RoomTile* m_tile;
	int m_health;
	int m_healt_max;

	float m_angle;
	bool m_direct_fire;
	float m_rof;//rate of fire
	float m_rof_timer;

	void Update(Time deltaTime);
	void UpdateRof(Time deltaTime);

	bool Fire(Time deltaTime, sf::Vector2f ship_position, float ship_angle, float distance_combat, Ship* target_ship, RoomTile* target_tile);

	RoomTile* GetFreeRoomTile(Room* room);
	RoomTile* GetFreeWeaponTile(Room* room, bool is_enemy);

	AmmoType m_ammo_type;
	Ship* m_ship;
};

#endif //SHIP_H_INCLUDED