#ifndef PREADATOR_H_INCLUDED
#define PREADATOR_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum PredatorState
{
	Predator_Swimming,
	Predator_Chasing,
	Predator_Attacking,
	Predator_Eating,
	NB_PREDATOR_STATES,
};

class Predator : public GameObject
{
public :
	Predator();
	Predator(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	void Init();

	virtual ~Predator();
	void update(sf::Time deltaTime) override;

	
	void Attack();
	void AddToPreys(GameObject* boid) override;
	void SetPrey(GameObject* prey) override;
	bool Eat(GameObject* prey) override;
	GameObject* GetPrey() override;

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;

	GameObject* m_prey;
	sf::Clock m_attack_duration_clock;
	sf::Clock m_attack_cooldown_clock;

	PredatorState m_state;
};

#endif // BOID_H_INCLUDED
