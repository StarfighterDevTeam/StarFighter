#ifndef RUDDER_H_INCLUDED
#define RUDDER_H_INCLUDED

#include "Game.h"
#include "Room.h"

class Ship;//forward declaration

class Rudder : public GameEntity
{
public:
	Rudder();
	~Rudder();

	void Update(Time deltaTime);

	RoomTile* m_tile;
	int m_health;
	int m_health_max;
	Ship* m_ship;

	float m_rotation_speed;
	float m_rotation_timer;

	GameEntity* m_lifebar;
	GameEntity* m_systembar;
};

#endif //RUDDER_H_INCLUDED