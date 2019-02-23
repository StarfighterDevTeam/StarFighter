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

#define TORPEDO_HULL_DAMAGE					50
#define TORPEDO_SPEED						1000.f
#define TORPEDO_INITIAL_SPEED				10.f
#define TORPEDO_ACCELERATION				100.f

#define SHRAPNEL_DAMAGE						15
#define SHRAPNEL_DELAY						0.1f

#define AMMO_WARP_DISTANCE_PER_SECOND		400.f

enum ShootPhase
{
	Shoot_Ougoing,
	Shoot_Hit,
};

enum AmmoType
{
	Ammo_CannonBall,
	Ammo_Torpedo,
	Ammo_Shrapnel,
};

class Ammo : public GameEntity
{
public:
	Ammo(AmmoType type, sf::Vector2f position, float angle, float distance_combat, Ship* target_ship, RoomTile* target_tile, sf::Vector2f target_position);
	~Ammo();

	AmmoType m_type;
	ShootPhase m_phase;

	Ship* m_target_ship;
	sf::Vector2f m_target_position;
	RoomTile* m_target_tile;
	float m_angle;
	sf::Vector2f m_speed;
	float m_ref_speed;
	float m_initial_speed;
	float m_current_speed;
	float m_acceleration;
	float m_distance_combat;
	float m_warp_timer;
	FX* m_FX_hit;
	FX* m_FX_miss;

	int m_damage;
	int m_hull_damage;
	int m_SHRAPNEL_DAMAGE;
	float m_radius;

	bool m_can_be_seen;

	void Update(Time deltaTime);
};

#endif //AMMO_H_INCLUDED