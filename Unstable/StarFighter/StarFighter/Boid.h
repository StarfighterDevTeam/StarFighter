#ifndef BOID_H_INCLUDED
#define BOID_H_INCLUDED

#include "Plancton.h"

struct Threat
{
	sf::Vector2f m_pos;
	sf::Clock m_clock;
	float m_angle;
	GameObject* m_object;
};

enum BoidState
{
	Boid_Swimming_Solo,
	Boid_Swimming_Flocked,
	Boid_Fornicating,
	Boid_Laying_Egg,
	NB_BOID_STATES,
};

class Boid : public LivingThing
{
public :
	Boid();
	Boid(sf::Vector2f position);
	void Init();

	virtual ~Boid();
	void update(sf::Time deltaTime) override;
	void Draw(RenderTarget& screen) override;

	void AddToBoidNeighbours(GameObject* boid) override;
	void ClearBoidNeighbours() override;
	void AddToBoidThreats(GameObject* predator) override;
	void AddToBoidThreats(sf::Vector2f pos) override;

	vector<Boid*> m_boid_neighbours;
	sf::Vector2f m_avg_position;
	sf::Vector2f GetAveragePosition();
	sf::Vector2f m_avg_speed;
	sf::Vector2f GetAverageSpeed();
	sf::Vector2f Separate();

	vector<Threat*> m_threats;

	sf::Vector2f Flee(sf::Vector2f threat_pos);
	float FleeByAngle(sf::Vector2f threat_pos);
	void UpdateThreats();

	void EggLaying();
	void Growing();
	bool Boid::IsGrown() override;

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;

	float m_egg_cooldown;
	sf::Clock m_egg_clock;
	sf::Clock m_between_two_eggs_clock;
	sf::Clock m_growth_clock;
	int m_eggs;
	int m_growth;
	BoidState m_state;

	sf::Vector2f m_oscillation_offset;
	sf::Clock m_oscillation_clock;

	sf::CircleShape m_debug_flocking_radius;
	sf::CircleShape m_debug_separation_radius;
};

#endif // BOID_H_INCLUDED
