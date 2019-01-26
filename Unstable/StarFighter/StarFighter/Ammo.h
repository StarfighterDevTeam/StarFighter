#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Room.h"
#include "WaterZone.h"
#include "FX.h"

#define CANNONBALL_SIZE						16.f

#define CANNONBALL_DAMAGE					10
#define CANNONBALL_SPEED					800.f
#define CANNONBALL_RADIUS					1

#define AMMO_WARP_DISTANCE_PER_SECOND		400.f

enum ShootPhase
{
	Shoot_Ougoing,
	Shoot_Hit,
};

enum AmmoType
{
	Ammo_CannonBall,
};

class Ammo : public GameEntity
{
public:
	Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile);
	~Ammo();

	AmmoType m_type;
	ShootPhase m_phase;

	Ship* m_target_ship;
	RoomTile* m_target_tile;
	float m_angle;
	sf::Vector2f m_speed;
	float m_ref_speed;
	float m_distance_combat;
	float m_warp_timer;
	FX* m_FX_hit;

	int m_damage;
	float m_radius;

	bool m_can_be_seen;

	void Update(Time deltaTime);
};

#endif //AMMO_H_INCLUDED