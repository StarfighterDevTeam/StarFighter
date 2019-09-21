#ifndef BEACON_H_INCLUDED
#define BEACON_H_INCLUDED

#include "EscortShip.h"

class Beacon : public SpatialObject
{
public:
	Beacon(sf::Vector2i sector_index, SpatialObject* trigger, bool isMissionObjective);
	~Beacon();
	void Draw(RenderTarget& screen) override;
	bool TryTrigger(GameObject* trigger) override;
	void Activate();

	vector<AIShip*> m_ships_to_create;
	SpatialObject* m_trigger;
	bool m_isMissionObjective;
};

#endif // BEACON_H_INCLUDED
