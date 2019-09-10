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
	Ship_Cruiser,
};

class AIShip : public Ship
{
public :
	AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe, bool dontStoreMe);
	~AIShip();
	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void SetHostility(Hostility hostility) override;
	bool CheckMarkingConditions() override;
	void GetHitByAmmo(GameObject* ammo) override;
	void GoTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction);
	SpatialObject* KeepTarget();
	void Death() override;
	void SetROE(RuleOfEngagement roe) override;

	ShipType m_ship_type;

	sf::Vector2f m_move_destination;
	SpatialObject* m_target;

	vector<SpatialObject*> m_allied_ships;
};

#endif // AISHIP_H_INCLUDED
