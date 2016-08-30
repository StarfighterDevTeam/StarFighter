#ifndef STARSHIP_H_INCLUDED
#define STARSHIP_H_INCLUDED

#include "StockEntity.h"
#include "Ore.h"

enum StarshipState
{
	StarshipState_Idle,
	StarshipState_MovingToLocation,
	StarshipState_Searching,
	StarshipState_Drilling,
	StarshipState_Extracting,
	StarshipState_Loading,
	StarshipState_CarryToBase,
	StarshipState_Unloading,
	StarshipState_Scanning,
};

class Starship : public StockEntity
{
public :
	Starship() {};
	Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Starship();
	void update(sf::Time deltaTime) override;

	static Starship* CreateStarship(string name);

	bool AssignToLocation(StockEntity* location);
	bool MoveToLocation(StockEntity* location);
	size_t LoadFuel(string ore_name, size_t quantity) override;
	size_t LoadRequiredPropulsion(StockEntity* location, size_t propulsion_missing, bool simulation);
	size_t AssignPropulsionToTravel(size_t distance);
	size_t ConsummePropulsion(size_t distance);
	bool ManagePropulsion();
	bool CheckIfArrivedAtDestination(sf::Time deltaTime);
	void SetStarshipState(StarshipState state);
	void Drill();
	void Scan();
	void Extract(Ore* ore);
	bool IsNewDrillAttemptAvailable();
	string GetBestAssignedPropulsionAvailable();
	size_t GetTotalWeight();
	size_t GetPropulsionRequired(GameObject* destination);

	sf::Vector2u GetCurrentZone();
	void UpdateZoneKnowledge();
	bool Scout();

	sf::Vector2u m_current_zone;

	StockEntity* m_target_location;
	StockEntity* m_base_location;
	StockEntity* m_task_location;

	StarshipState m_state;
	size_t m_propulsion_assigned;
	bool m_arrived_at_distination;
	map<string, size_t> m_cost;
	float m_scout_range;

	//Weapon* m_weapon;
	int m_armor;
	int m_armor_max;
	float m_speed_max;
	size_t m_weight;
	size_t m_total_weight;
	size_t m_propulsion;
	string m_current_fuel_type;
	map<string, size_t> m_fuel_assigned;
	float m_propulsion_speed_bonus;
	size_t m_nb_drills;
	float m_drill_sucess_rate_bonus;
	float m_extraction_duration_bonus;
	size_t m_current_drill_attempts;
	sf::Clock m_drill_clock;
	float m_drill_duration;
	sf::Clock m_extraction_clock;
	Ore* m_ore_found;
	sf::Clock m_scan_clock;
};

#endif // STARSHIP_H_INCLUDED
