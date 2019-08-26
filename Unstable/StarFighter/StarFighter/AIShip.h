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
	void Update(sf::Time deltaTime) override;
	void SetHostility(HostilityLevel hostility);

	ShipType m_ship_type;
	sf::RectangleShape m_targeting_rect;
};

#endif // AISHIP_H_INCLUDED
