#ifndef AISHIP_H_INCLUDED
#define AISHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Ship.h";

enum ShipType
{
	Ship_Alpha,
};

class AIShip : public Ship
{
public :
	AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, HostilityLevel hostility);
	~AIShip();
	void Update(sf::Time deltaTime) override;
	void SetHostility(HostilityLevel hostility);
	bool GetHitByAmmo(GameObject* ammo) override;

	ShipType m_ship_type;
	Marker* m_marker;
	HostilityLevel m_hostility;
};

#endif // AISHIP_H_INCLUDED
