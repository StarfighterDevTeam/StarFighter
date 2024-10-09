#include "HumanShip.h"

extern Game* CurrentGame;

using namespace sf;

HumanShip::HumanShip(sf::Vector2i sector_index, bool is_local_player) : Ship()
{
	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	string textureName = is_local_player == true ? "2D/ship_alpha_blue.png" : "2D/ship_alpha_green.png";
	Init(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), textureName, sf::Vector2f(68, 84), 3, 1);

	//Flight model
	m_speed_max = 800;
	m_acceleration_max = 1500;
	m_turn_speed = 160;
	m_braking_max = 3000;
	m_idle_decelleration = 500;

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
		(*CurrentGame).m_playerShip->SendNetworkPacket(ammo_, false);
}