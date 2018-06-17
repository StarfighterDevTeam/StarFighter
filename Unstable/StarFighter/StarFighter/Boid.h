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

	vector<Boid*> m_boid_neighbours;
	sf::Vector2f m_avg_position;
	sf::Vector2f GetAveragePosition();
	sf::Vector2f m_avg_speed;
	sf::Vector2f GetAverageSpeed();
	sf::Vector2f Separate();

	sf::Vector2f Flee(sf::Vector2f threat_pos);
	vector<Threat> m_threats;
	void UpdateThreats();

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;
};

#endif // BOID_H_INCLUDED
