#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Weapon.h"
#include "Island.h"
#include "Compass.h"
#include "CrewInterface.h"
#include "RewardInterface.h"
#include "PrisonersChoiceInterface.h"
#include "DockyardInterface.h"
#include "CrewOverboardInterface.h"
#include "CrewUnboardInterface.h"
#include "Ship.h"

#include <fstream>

#define COMPASS_MODE					false

class Warship : public Ship
{
public:
	Warship(DMS_Coord coord);
	void Init();
	~Warship();

	float m_desired_angle;
	float m_angle_speed;
	int m_upkeep_costs[NB_UPKEEP_COSTS];
	bool m_can_open_new_menu;
	vector<CrewMember*> m_crew_unboarding;
	vector<Commodity*> m_holds;
	int m_holds_capacity;
	int m_sonarRange;

	struct Compass m_compass;
	struct CrewInterface m_crew_interface;
	struct PrisonersChoiceInterface m_prisoners_choice_interface;
	struct CrewOverboardInterface m_crew_overboard_interface;
	struct CrewUnboardInterface m_crew_unboard_interface;
	struct RewardInterface m_reward_interface;
	struct DockyardInterface m_dockyard_interface;

	void UpdateOLD(Time deltaTime, bool tactical_combat);

	void UpdateUpkeepCosts();
	bool HasCommodity(CommodityType commodity) override;

	bool ApplyUpgrade(string upgrade_type);
	void UpdateRooms();

	int GetSonarRange() override { return m_sonarRange; };
};

#endif //WARSHIP_H_INCLUDED