#ifndef PREADATOR_H_INCLUDED
#define PREADATOR_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Predator : public GameObject
{
public :
	Predator();
	Predator(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	void Init();

	virtual ~Predator();
	void update(sf::Time deltaTime) override;

	void UpdatePrey();
	void Attack();
	void AddToPreys(GameObject* boid) override;
	bool HasPrey() override;
	void Eat(GameObject* prey) override;

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;

	GameObject* m_prey;
	sf::Clock m_eating_clock;
	sf::Clock m_chasing_clock;
};

#endif // BOID_H_INCLUDED
