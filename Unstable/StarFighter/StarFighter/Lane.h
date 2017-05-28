#ifndef LANE_H_INCLUDED
#define LANE_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"

class Lane : public GameObject
{
public :
	Lane(GameObject* spawner);
	void update(sf::Time deltaTime) override;

	GameObject* m_spawner;
	float m_lane_angle;
};

#endif // LANE_H_INCLUDED
