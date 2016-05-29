#ifndef STARSHIP_H_INCLUDED
#define STARSHIP_H_INCLUDED

#include "Location.h"
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
};

class Starship : public GameObject
{
public :
	Starship() {};
	Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Starship();
	void update(sf::Time deltaTime) override;

	bool AssignToLocation(Location* location);
	size_t LoadFuel(string ore_name, size_t quantity);
	bool LoadRequiredPropulsion(Location* location, size_t propulsion_missing);
	size_t AssignPropulsionToTravel(size_t distance);
	size_t ConsummePropulsion(size_t distance);
	bool ManagePropulsion();
	bool CheckIfArrivedAtDestination(sf::Time deltaTime);
	virtual void SetStarshipState(StarshipState state);
	string GetBestPropulsionAvailable();

	Location* m_location;
	StarshipState m_state;
	size_t m_assigned_propulsion;
	bool m_arrived_at_distination;
	string m_display_name;

	int m_scout_range;

	//Weapon* m_weapon;

	int m_armor;
	size_t m_fuel;
	size_t m_fuel_max;
	size_t m_propulsion;
	string m_current_fuel_type;
	map<string, size_t> m_fuel_tanks;
	map<string, size_t> m_fuel_assigned;
	float m_speed_max;
	float m_propulsion_speed_bonus;

	int m_stock_max;
	int m_stock;

	size_t m_nb_drills;
	size_t m_current_drill_attempts;

	sf::Clock m_drill_clock;
	float m_drill_duration;

	sf::Clock m_extraction_clock;
};

class Miner : public Starship
{
public:
	Miner() {};
	Miner(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Miner();
	void update(sf::Time deltaTime) override;

	void Drill();
	void Extract(Ore* ore);
	Ore* GetRandomOre(OreField* ore_field);
	void LoadOre(Ore* ore);
	void SetStarshipState(StarshipState state) override;
	bool IsNewDrillAttemptAvailable();
	void UnloadCarriage(Location* location);

	float m_drill_sucess_rate_bonus;
	float m_extraction_duration_bonus;
	Ore* m_ore_found;

	map<string, size_t> m_ores_carried;
};

#endif // STARSHIP_H_INCLUDED
