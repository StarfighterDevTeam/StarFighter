#ifndef STARSHIP_H_INCLUDED
#define STARSHIP_H_INCLUDED

#include "StockEntity.h"
#include "Ore.h"

enum StarshipState
{
	StarshipState_Idle,
	StarshipState_MovingToLocation,
	StarshipState_MovingToZone,
	StarshipState_Scouting,
	StarshipState_Drilling,
	StarshipState_Extracting,
	StarshipState_Loading,
	StarshipState_CarryToBase,
	StarshipState_Unloading,
	StarshipState_Scanning,
};

enum StarshipMission
{
	StarshipMission_Idle,
	StarshipMission_MoveToLocation,
	StarshipMission_Scout,
	StarshipMission_Scan,
	StarshipMission_Drill,
	StarshipMission_Load,
	StarshipMission_Unload,
};

class Starship : public StockEntity
{
public :
	Starship() {};
	Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Starship();
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderTexture& screen);

	static Starship* CreateStarship(string name);

	//bool AssignToLocation(StockEntity* location);
	//bool AssignMission(StockEntity* location);
	//bool MoveToLocation(StockEntity* location);
	size_t LoadFuelTank(string fuel_name, size_t quantity) override;
	//size_t LoadRequiredPropulsion(StockEntity* location, size_t propulsion_missing, bool simulation);
	//size_t AssignPropulsionToTravel(size_t distance);
	//size_t ConsummePropulsion(size_t propulsion_to_consumme);
	//bool ManagePropulsion();
	bool CheckIfArrivedAtDestination(sf::Time deltaTime);
	void SetStarshipState(StarshipState state);
	Ore* Drill();
	bool Scan(StockEntity* entity);
	bool Extract(Ore* ore);
	//string GetBestAssignedPropulsionAvailable();
	size_t GetPropulsionRequired(GameObject* destination);

	bool AssignMission(StarshipMission mission, sf::Vector2f destination, StockEntity* task_location = NULL, StockEntity* base_location = NULL);
	void ManageMission(sf::Time deltaTime);
	bool ArrivingAtDestination(sf::Time deltaTime);
	bool CheckIfEnoughFuelToDestination(sf::Vector2f destination);
	size_t GetFuelCostToDestination(sf::Vector2f destination);
	size_t GetPropulsionAvailable();


	sf::Vector2u GetCurrentZone();
	void UpdateZoneKnowledge();
	bool Scout();

	sf::Vector2u m_current_zone;

	StockEntity* m_current_destination_location;
	StockEntity* m_current_location;
	StockEntity* m_mission_base_location;
	StockEntity* m_mission_task_location;
	StarshipMission m_mission;
	sf::Vector2f m_current_destination_coordinates;
	bool m_loop_mission;

	StarshipState m_state;
	bool m_arrived_at_destination;
	map<string, size_t> m_cost;
	float m_scout_range;
	pair<string, size_t> m_fuel_tank;
	size_t m_fuel_tank_max;

	//Weapon* m_weapon;
	int m_armor;
	int m_armor_max;
	float m_speed_max;
	size_t m_weight;
	size_t m_propulsion;
	string m_current_fuel_type;
	float m_propulsion_speed_bonus;
	size_t m_nb_drills;
	float m_drill_sucess_rate_bonus;
	float m_extraction_duration_bonus;
	size_t m_drill_attempts;
	sf::Clock m_drill_clock;
	float m_drill_duration;
	sf::Clock m_extraction_clock;
	Ore* m_ore_found;
	sf::Clock m_scan_clock;

	CircleShape m_scout_circle;
	CircleShape m_travel_circle;
};

#endif // STARSHIP_H_INCLUDED
