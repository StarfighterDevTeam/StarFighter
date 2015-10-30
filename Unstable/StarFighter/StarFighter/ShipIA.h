#ifndef SHIPIA_H_INCLUDED
#define SHIPIA_H_INCLUDED

#include "Ship.h"
#include "Goal.h"

enum IADifficultyLevel
{
	IAEasy,
	IAMedium,
	IAHard,
};

class ShipIA : public Ship
{
public :
	ShipIA();
	ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	ShipIA(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	virtual ~ShipIA();
	
	//IA SPECIFIC
	//# actions
	void update(sf::Time deltaTime) override;
	void IA_MoveToPosition(sf::Vector2f position, sf::Time deltaTime);
	void IA_ShootToPosition(sf::Vector2f position);

	//# utilitary
	Discoball* m_target_discoball;
	Goal* m_target_goal;
	bool SetTargetDiscoball();
	bool SetTargetGoal(bool own_goal);
	void(*foo)(int);
	void SetIADifficultyLevel(IADifficultyLevel IA_level);
	sf::Time GetReactionTime(IADifficultyLevel IA_level);
	float GetThrowPrecision(IADifficultyLevel IA_level);
	void ActivateIA(bool activate);

private:
	bool m_IA_activated;
	IADifficultyLevel m_IA_level;
	sf::Clock reaction_clock;
};

#endif // SHIPIA_H_INCLUDED
