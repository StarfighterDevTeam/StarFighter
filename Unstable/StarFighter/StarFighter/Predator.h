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

	float m_change_dir_time;
	sf::Clock m_change_dir_clock;

	GameObject* m_prey;
};

#endif // BOID_H_INCLUDED
