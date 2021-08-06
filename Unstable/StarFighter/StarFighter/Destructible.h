#ifndef DESTRUCTIBLE_H_INCLUDED
#define DESTRUCTIBLE_H_INCLUDED

#include "Planet.h"

class Destructible : public SpatialObject
{
public :
	Destructible();
	virtual ~Destructible();
	void Update(sf::Time deltaTime) override;
	void GetHitByAmmo(GameObject* ammo, bool send_network_packet = true) override;
	void GetHitByObject(GameObject* object) override;
	virtual void Death();

	Loot* CreateLoot(int money);

	int m_collision_damage;
	int m_health;
	int m_health_max;

	float m_hit_feedback_timer;
	float m_hit_immunity_timer;
};

#endif // DESTRUCTIBLE_H_INCLUDED
