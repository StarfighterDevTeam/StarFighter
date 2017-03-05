#ifndef WALKER_H_INCLUDED
#define WALKER_H_INCLUDED

#include "Agent.h"

enum WalkerState
{
	Walker_Undecided,
	Walker_Idle,
	Walker_Walk,
};

enum WalkerAwareness
{
	Walker_Unaware,
	Walker_Casual,
	Walker_Suspicious,
	Walker_Angry,
	Walker_Mad,
};

class Walker : public Agent
{
public :
	Walker();
	Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Walker();
	void update(sf::Time deltaTime) override;

	void SetRandomWalkRoutine();
	void SetRandomIdleRoutine();

	WalkerState m_state;
	WalkerAwareness m_awareness;
	sf::Clock m_phaseClock;
	float m_phaseTime;
	float m_standardSpeed;
};

#endif // WALKER_H_INCLUDED
