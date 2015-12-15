#ifndef GOAL_H_INCLUDED
#define GOAL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum GoalAnimations
{
	GoalNormal,
	GoalHighlighted,
};

class Goal : public GameObject
{
public :
	Goal();
	Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size);
	Goal(Teams team, sf::Vector2f position, sf::Vector2f size);
	virtual ~Goal();

	void CollisionResponse(Time deltaTime) override;

	Teams m_team;
	int m_stroke_size;
	Glow* m_glow_effect;
};

#endif // GOAL_H_INCLUDED
