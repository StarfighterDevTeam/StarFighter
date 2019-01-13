#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Room.h"
#include "WaterZone.h"

#define CANNONBALL_SIZE						16.f
#define CANNONBALL_SPEED					800.f

#define AMMO_WARP_DISTANCE_PER_SECOND		400.f

enum ShootPhase
{
	Shoot_Ougoing,
	Shoot_Warping,
	Shoot_Incoming,
};

enum AmmoType
{
	Ammo_CannonBall,
};

class Ammo : public GameEntity
{
public:
	Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat);

	AmmoType m_type;
	ShootPhase m_phase;

	float m_angle;
	sf::Vector2f m_speed;
	float m_ref_speed;
	float m_distance_combat;
	float m_warp_timer;

	bool m_can_be_seen;

	void Update(Time deltaTime, DMS_Coord warship_DMS);
};

#endif //AMMO_H_INCLUDED