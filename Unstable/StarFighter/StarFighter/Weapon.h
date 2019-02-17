#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"
#include "Ammo.h"
#include "Room.h"
#include "CrewMember.h"
#include "WaterZone.h"

class Ship;//forward declaration

#define CANNON_HEALTH_MAX		30
#define CANNON_RATE_OF_FIRE		8.f
#define CANNON_ANGLESPEED		20.f

#define TORPEDO_RATE_OF_FIRE		10.f
#define TORPEDO_ANGLESPEED		10.f

#define SHARPNEL_RATE_OF_FIRE	12.f
#define SHARPNEL_ANGLESPEED		30.f

enum WeaponType
{
	Weapon_Cannon,
	Weapon_Torpedo,
	Weapon_Sharpnel,
};

class Weapon : public GameEntity
{
public:
	Weapon(WeaponType type, bool is_enemy);
	~Weapon();

	WeaponType m_type;
	RoomTile* m_tile;
	Room* m_target_room;
	int m_health;
	int m_health_max;

	float m_angle;
	float m_rof;//rate of fire
	float m_rof_timer;
	float m_angle_speed;

	bool m_is_guided;//true=cannot miss

	void Update(Time deltaTime);

	void UpdateRof(Time deltaTime);
	float GetRof();
	bool CanFire();

	void Fire(float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile, sf::Vector2f target_position);

	RoomTile* GetFreeRoomTile(Room* room);
	RoomTile* GetFreeWeaponTile(Room* room, bool is_enemy);

	AmmoType m_ammo_type;
	Ship* m_ship;

	GameEntity* m_lifebar;
	GameEntity* m_rofbar;
};

#endif //SHIP_H_INCLUDED