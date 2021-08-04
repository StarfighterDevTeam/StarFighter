#ifndef ONLINESHIP_H_INCLUDED
#define ONLINESHIP_H_INCLUDED

#include "EscortShip.h";

class HumanShip : public AIShip
{
public :
	HumanShip(ShipType ship_type, sf::Vector2i sector_index, float heading);
	virtual ~HumanShip();
	void Death() override;
	bool CheckMarkingConditions() override;
};

#endif // ONLINESHIP_H_INCLUDED
