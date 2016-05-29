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
	bool ManageFuel();
	bool CheckIfArrivedAtDestination(sf::Time deltaTime);
	virtual void SetStarshipState(StarshipState state);

	Location* m_location;
	StarshipState m_state;
	size_t m_assigned_fuel;
	bool m_arrived_at_distination;
	string m_display_name;

	int m_scout_range;

	//Weapon* m_weapon;

	int m_armor;

	size_t m_fuel_max;
	size_t m_fuel;
	float m_speed_max;

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
	size_t UnloadCarriage();

	float m_drill_sucess_rate_bonus;
	float m_extraction_duration_bonus;
	Ore* m_ore_found;

	vector<Ore*> m_ores_carried;
};

#endif // STARSHIP_H_INCLUDED
