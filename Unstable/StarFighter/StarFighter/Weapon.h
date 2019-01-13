#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"
#include "Ammo.h"
#include "Room.h"
#include "WaterZone.h"

class Ship;//forward declaration

enum WeaponType
{
	Weapon_Cannon,
};

class Weapon : public GameEntity
{
public:
	Weapon(WeaponType type);
	~Weapon();

	WeaponType m_type;
	RoomTile* m_tile;

	float m_angle;
	bool m_direct_fire;

	void Update(Time deltaTime);

	bool Fire(Time deltaTime, sf::Vector2f ship_position, float ship_angle, float distance_combat);

	RoomTile* GetFreeRoomTile(Room* room);

	Ammo* m_ammo;
	Ship* m_ship;
};

#endif //SHIP_H_INCLUDED