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
	void Init();
	virtual ~ShipIA();
	
	//IA SPECIFIC
	//# actions
	void update(sf::Time deltaTime) override;
	void IA_MoveToPosition(sf::Vector2f position, sf::Time deltaTime);
	void IA_MoveToObject(GameObject* object, sf::Time deltaTime, bool anticipation = false);
	void IA_ShootToPosition(sf::Vector2f position);
	void IA_GuardPosition(sf::Vector2f position, float distance, sf::Time deltaTime);

	//# utilitary
	Discoball* m_target_discoball;
	Goal* m_target_goal;
	bool SetTargetDiscoball();
	bool SetTargetGoal(bool own_goal);
	bool SetTargetTeamMate(bool only_unmarked = true);
	void SetIADifficultyLevel(IADifficultyLevel IA_level);
	sf::Time GetReactionTime(IADifficultyLevel IA_level);
	float GetThrowPrecision(IADifficultyLevel IA_level);
	void ActivateIA(bool activate);
	sf::Vector2f GetDefensivePosition(sf::Vector2f position, float distance, GameObject* object_to_intercept, bool anticipation = false);
	float GetAngleVariationToObject(GameObject* object_to_intercept);
	bool isTargetGoalGuarded();
	bool isTargetBallContested();
	bool isOffside();
	static GameObjectType GetOpponentGameObjectType(Ship* player);
	static GameObjectType GetOwnGameObjectType(Ship* player);

private:
	bool m_IA_activated;
	IADifficultyLevel m_IA_level;
	sf::Clock reaction_clock;
};

#endif // SHIPIA_H_INCLUDED
