#include "HumanShip.h"

extern Game* CurrentGame;

using namespace sf;

HumanShip::HumanShip(sf::Vector2i sector_index, bool is_local_player) : Ship()
{
	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	string textureName = is_local_player == true ? "2D/ship_alpha_blue.png" : "2D/ship_alpha_green.png";
	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(68, 84), 3, 1);

	//Flight model
	m_speed_max = 800;
	m_acceleration_max = 3000;
	m_turn_speed = 240;
	m_braking_max = 3000;
	m_idle_decelleration = 1000;

	//combat
	m_health_max = 50;
	m_shield_max = 20;
	m_shield_range = 100;
	m_shield_regen = 1.5;
	m_isReflectingShots = false;
	m_energy_max = 100;
	m_energy_regen = 10;

	m_gravitation_range = 0;//300
	m_gravitation_strength = 70;//fine_tuned for player m_speed_max = 800; m_acceleration_max = 2000;

	InitShip();
}

HumanShip::~HumanShip()
{
	
}

void HumanShip::Death()
{
	//FX death
	FX* new_FX = new FX(FX_Death, m_position);
	(*CurrentGame).addToScene(new_FX, FX_Layer, BackgroundObject, true);

	//Respawn
	Replenish();
}

bool HumanShip::CheckMarkingConditions()
{
	return true;
}

void HumanShip::GetHitByAmmo(GameObject* ammo, bool send_network_packet)
{
	Ship::GetHitByAmmo(ammo, send_network_packet);

	Ammo* ammo_ = (Ammo*)ammo;
	if (send_network_packet == true)
		SendNetworkPacket(ammo_, false);
}

//NETWORK
void HumanShip::SendNetworkPacket(NetworkPacketType type, Ammo* ammo, bool is_local_player)
{
	sf::Packet packet;
	packet << (int)type;

	switch (type)
	{
		case Packet_PlayerShipUpdate:
		{
			GameObject* ship = (*CurrentGame).m_playerShip;
			packet << ship->m_position.x << ship->m_position.y << ship->m_heading << ship->m_speed.x << ship->m_speed.y;

			break;
		}
		case Packet_AmmoCreation:
		{
			if (ammo != NULL)
			{
				ammo->m_online_id = RandomizeIntBetweenValues(1, 1000);//unique identifier of the object across online network
				packet << ammo->m_online_id << (int)ammo->m_ammo_type << ammo->m_position.x << ammo->m_position.y << ammo->m_heading << ammo->m_lifespan << ammo->m_damage << (int)ammo->m_collider << ammo->m_speed.x << ammo->m_speed.y;
			}

			break;
		}
		case Packet_AmmoImpact:
		{
			if (ammo != NULL)
			{
				packet << is_local_player << ammo->m_online_id;
			}

			break;
		}
	}

	(*CurrentGame).m_socket.send(packet, (*CurrentGame).m_ip, (*CurrentGame).m_port_send);
	//(*CurrentGame).m_socket.send(packet, "127.0.0.1", (*CurrentGame).m_port_receive);//Local test
}

void HumanShip::SendNetworkPacket(Ammo* ammo, bool is_local_player)
{
	SendNetworkPacket(Packet_AmmoImpact, ammo, is_local_player);
}

void HumanShip::SendNetworkPacket(Ammo* ammo)
{
	SendNetworkPacket(Packet_AmmoCreation, ammo);
}