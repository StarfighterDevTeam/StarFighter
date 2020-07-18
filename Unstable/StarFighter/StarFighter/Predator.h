#ifndef PREADATOR_H_INCLUDED
#define PREADATOR_H_INCLUDED

#include "Boid.h"

enum PredatorState
{
	Predator_Swimming,
	Predator_Chasing,
	Predator_Attacking,
	Predator_Eating,
	NB_PREDATOR_STATES,
};

class Predator : public LivingThing
{
public :
	Predator();
	Predator(sf::Vector2f position);
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
