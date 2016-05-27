#ifndef STARSHIP_H_INCLUDED
#define STARSHIP_H_INCLUDED

#include "Location.h"
#include "Ore.h"

class Starship : public GameObject
{
public :
	Starship() {};
	Starship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	virtual ~Starship();
	void update(sf::Time deltaTime) override;

	string m_display_name;

	int m_scout_range;

	//Weapon* m_weapon;

	int m_armor;

	float m_fuel_max;
	float m_fuel;
	float m_speed_max;

	int m_stock_max;
	int m_stock;

	size_t m_nb_drills;
	size_t m_current_drill_attempts;

	sf::Clock m_drill_clock;
	float m_drill_duration;

	sf::Clock m_extraction_clock;

	Location* m_location;
};

enum MiningState
{
	Mining_MovingToLocation,
	Mining_Searching,
	Mining_Drilling,
	Mining_Extracting,
	Mining_Loading,
	Mining_CarryToBase,
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
	void SetMiningState(MiningState state);
	bool IsNewDrillAttemptAvailable();

	MiningState m_mining_state;
	float m_drill_sucess_rate_bonus;
	float m_extraction_duration_bonus;
	Ore* m_ore_found;

	vector<Ore*> m_ores_carried;
};

#endif // STARSHIP_H_INCLUDED
