#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Weapon.h"
#include "Island.h"
#include "Compass.h"
#include "CrewInterface.h"
#include "PrisonersChoiceInterface.h"
#include "CrewOverboardInterface.h"
#include "Ship.h"

#include <fstream>



#define COMPASS_MODE					false

enum Resource_Meta
{
	Resource_Gold,
	Resource_Fish,
	Resource_Mech,
	Resource_Fidelity,
	Resource_Days,
	NB_RESOURCES_TYPES,
};

class Warship : public Ship
{
public:
	Warship(DMS_Coord coord);

	void Init();

	~Warship();

	float m_desired_angle;
	float m_angle_speed;
	struct Compass m_compass;
	struct CrewInterface m_crew_interface;
	struct PrisonersChoiceInterface m_prisoners_choice_interface;
	struct CrewOverboardInterface m_crew_overboard_interface;

	void UpdateOLD(Time deltaTime, bool tactical_combat);

	int m_resources[NB_RESOURCES_TYPES];
};

#endif //WARSHIP_H_INCLUDED