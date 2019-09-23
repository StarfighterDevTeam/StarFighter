#ifndef LOOT_H_INCLUDED
#define LOOT_H_INCLUDED

#include "Marker.h"

class Loot : public SpatialObject
{
public :
	Loot(int money);
	virtual ~Loot();
	void Update(sf::Time deltaTime) override;
	void GetHitByObject(GameObject* object) override;

	int m_money;
};

#endif // LOOT_H_INCLUDED
