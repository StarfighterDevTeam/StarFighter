#ifndef ESCORTSHIP_H_INCLUDED
#define ESCORTSHIP_H_INCLUDED

#include "AIShip.h";

class EscortShip : public AIShip
{
public :
	EscortShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, AIShip* escorted_ship, sf::Vector2f escort_offset);
	~EscortShip();
	void Update(sf::Time deltaTime) override;
	void SetROE(RuleOfEngagement roe) override;
	AIShip* GetEscortedShip() override;
	bool CheckMarkingConditions() override;

	AIShip* m_escorted_ship;
	sf::Vector2f m_escorted_offset;
};

#endif // ESCORTSHIP_H_INCLUDED
