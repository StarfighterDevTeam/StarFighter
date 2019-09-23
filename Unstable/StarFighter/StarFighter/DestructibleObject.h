#ifndef DESTRUCTIBLEOBJECT_H_INCLUDED
#define DESTRUCTIBLEOBJECT_H_INCLUDED

#include "Planet.h"

class DestructibleObject : public SpatialObject
{
public :
	DestructibleObject();
	virtual ~DestructibleObject();
	void Update(sf::Time deltaTime) override;
	void GetHitByAmmo(GameObject* ammo) override;
	void GetHitByObject(GameObject* object) override;
	virtual void Death();

	Loot* CreateLoot(int money);

	int m_collision_damage;
	int m_health;
	int m_health_max;

	float m_hit_feedback_timer;
	float m_hit_immunity_timer;
};

#endif // DESTRUCTIBLEOBJECT_H_INCLUDED
