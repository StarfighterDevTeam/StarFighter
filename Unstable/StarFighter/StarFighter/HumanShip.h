#ifndef ONLINESHIP_H_INCLUDED
#define ONLINESHIP_H_INCLUDED

#include "EscortShip.h"

class HumanShip : public Ship
{
public :
	HumanShip(sf::Vector2i sector_index, bool is_local_player);
	virtual ~HumanShip();

	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void Death() override;
	bool CheckMarkingConditions() override;
	void GetHitByAmmo(GameObject* ammo, bool send_network_packet = true) override;

	GameObject* m_thruster_trail;
};

#endif // ONLINESHIP_H_INCLUDED
