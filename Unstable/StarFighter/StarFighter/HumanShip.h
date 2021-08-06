#ifndef ONLINESHIP_H_INCLUDED
#define ONLINESHIP_H_INCLUDED

#include "EscortShip.h";

class HumanShip : public Ship
{
public :
	HumanShip(sf::Vector2i sector_index, bool is_local_player);
	virtual ~HumanShip();
	void Death() override;
	bool CheckMarkingConditions() override;
	void GetHitByAmmo(GameObject* ammo, bool send_network_packet = true) override;

	void SendNetworkPacket(NetworkPacketType type, Ammo* ammo = NULL, bool is_local_player = false);
	void SendNetworkPacket(Ammo* ammo, bool is_local_player);
	void SendNetworkPacket(Ammo* ammo);
};

#endif // ONLINESHIP_H_INCLUDED
