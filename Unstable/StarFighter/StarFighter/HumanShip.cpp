#include "HumanShip.h"

extern Game* CurrentGame;

using namespace sf;

HumanShip::HumanShip(ShipType ship_type, sf::Vector2i sector_index, float heading) : AIShip(ship_type, sector_index, heading, Hostility_Ally, ROE_HoldFire)
{
	
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