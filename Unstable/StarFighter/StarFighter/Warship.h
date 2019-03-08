#ifndef WARSHIP_H_INCLUDED
#define WARSHIP_H_INCLUDED

#include "Game.h"

#include "CrewMember.h"
#include "Weapon.h"
#include "Island.h"
#include "Compass.h"
#include "CrewInterface.h"
#include "CombatInterface.h"
#include "PrisonersChoiceInterface.h"
#include "CrewOverboardInterface.h"
#include "Ship.h"

#include <fstream>

#define	CRUISE_SPEED					50.f
#define ANGLE_SPEED						30.f

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
	~Warship();

	float m_desired_angle;
	float m_angle_speed;
	struct Compass m_compass;
	struct CrewInterface m_crew_interface;
	struct PrisonersChoiceInterface m_prisoners_choice_interface;
	struct CrewOverboardInterface m_crew_overboard_interface;

	float GetDistanceSquaredInSecondsDMS(WaterTile* tile);
	bool CanViewWaterTile(WaterTile* tile);
	int GetDistanceToWaterTile(WaterTile* tile);
	float GetDistanceFloatToWaterTile(WaterTile* tile);
	bool SetSailsToWaterTile(WaterTile* tile);
	bool IsOnlyWaterInsideRectangle(WaterTile* tileA, WaterTile* tileB);

	void Update(Time deltaTime, bool tactical_combat);

	struct CombatInterface m_combat_interface[2];

	int m_resources[NB_RESOURCES_TYPES];

private:
	void FindShortestPath(WaterTile* tileA, WaterTile* tileB);
	void IteratePathFinding(WaterTile* tileA, WaterTile* tileB);
	list<WaterTile*> m_closed_list_pathfind;
	list<WaterTile*> m_open_list_pathfind;

	bool RayTracingContainsIsland(WaterTile* tileA, WaterTile* tileB);
	bool RayTracingContainsIslandForPathfind(WaterTile* tileA, WaterTile* tileB);
	void RayTracingGetPath(WaterTile* tileA, WaterTile* tileB);
};

#endif //WARSHIP_H_INCLUDED