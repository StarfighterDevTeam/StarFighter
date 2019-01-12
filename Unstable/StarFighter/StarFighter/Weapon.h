#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include "Game.h"
#include "Room.h"

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

	RoomTile* GetFreeRoomTile(Room* room);
};

#endif //SHIP_H_INCLUDED