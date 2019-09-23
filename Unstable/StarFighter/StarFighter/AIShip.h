#ifndef AISHIP_H_INCLUDED
#define AISHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "SFTextPop.h"

#include "Ship.h";

#define AMBUSH_ENGAGEMENT_DISTANCE			(REF_WINDOW_RESOLUTION_X * 0.5)
#define FIREATWILL_ENGAGEMENT_DISTANCE		(REF_WINDOW_RESOLUTION_X * 2)

enum ShipType
{
	Ship_Alpha,
	Ship_Sigma,
	Ship_Cruiser,
	Ship_Convoy,
};

class AIShip : public Ship
{
public :
	AIShip(ShipType ship_type, sf::Vector2i sector_index, float heading, Hostility hostility, RuleOfEngagement roe);
	virtual ~AIShip();
	void Update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;
	void SetHostility(Hostility hostility) override;
	bool CheckMarkingConditions() override;
	void GetHitByAmmo(GameObject* ammo) override;
	void GetHitByObject(GameObject* object) override;
	void GoTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction);
	void TurnTo(sf::Vector2f position, sf::Time deltaTime, sf::Vector2f& inputs_direction);
	SpatialObject* KeepTarget(const float dist_max);
	void Death() override;
	void SetROE(RuleOfEngagement roe) override;
	void UpdateAlliedShips() override;
	void OffsetMoveDestinationToAvoidAlliedShips(const float dx, const float dy);

	ShipType m_ship_type;

	sf::Vector2f m_move_destination;
	sf::Vector2f* m_scripted_destination;
	bool m_move_clockwise;
	SpatialObject* m_target;
	RuleOfEngagement m_native_ROE;

	vector<SpatialObject*> m_dynamic_allied_ships;
	vector<SpatialObject*> m_scripted_allied_ships;
};

#endif // AISHIP_H_INCLUDED
