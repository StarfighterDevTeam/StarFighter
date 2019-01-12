#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Room.h"
#include "WaterZone.h"

#define CANNONBALL_SIZE			16.f
#define CANNONBALL_SPEED		200.f

enum MapView
{
	Map_Rooms,
	Map_Water,
};

enum AmmoType
{
	Ammo_CannonBall,
};

class Ammo : public GameEntity
{
public:
	Ammo(AmmoType type, sf::Vector2f position, float angle, MapView view);

	AmmoType m_type;
	MapView m_view;

	float m_angle;
	sf::Vector2f m_speed;
	float m_ref_speed;

	bool m_can_be_seen;

	void Update(Time deltaTime, DMS_Coord warship_DMS);
};

#endif //AMMO_H_INCLUDED