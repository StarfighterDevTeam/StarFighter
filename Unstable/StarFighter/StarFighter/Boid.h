#ifndef BOID_H_INCLUDED
#define BOID_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

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


class Boid : public GameObject
{
public :
	Boid();
	Boid(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	void Init();

	virtual ~Boid();
	void update(sf::Time deltaTime) override;

	void AddToBoidNeighbours(GameObject* boid) override;
	void ClearBoidNeighbours() override;
	void AddToBoidThreats(GameObject* predator) override;

	vector<Boid*> m_boid_neighbours;
	sf::Vector2f m_avg_position;
	sf::Vector2f GetAveragePosition();
	sf::Vector2f m_avg_speed;
	sf::Vector2f GetAverageSpeed();
	sf::Vector2f Separate();

	sf::Vector2f Flee(sf::Vector2f threat_pos);
	vector<Threat> m_threats;
	void UpdateThreats();

	void EggLaying();
	void Growing();
	bool Boid::IsGrown() override;

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;

	sf::Clock m_egg_clock;
	sf::Clock m_between_two_eggs_clock;
	sf::Clock m_growth_clock;
	int m_eggs;
	int m_growth;
	BoidState m_state;
};

#endif // BOID_H_INCLUDED
