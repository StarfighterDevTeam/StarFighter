#ifndef ENEMYGENERATOR_H_INCLUDED
#define ENEMYGENERATOR_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <assert.h>

const float spawnCostCollateralMultiplierTable[NB_HAZARD_LEVELS] = { 0.35, 0.30, 0.25, 0.20, 0.15 };

class EnemyGenerator
{
public:
	EnemyGenerator::EnemyGenerator(float m_spawnCost, int m_enemyClass, float m_spawnRepeatProbability = 0, float m_spawnMissProbability = 0);
	sf::Clock currentClock;
	float spawnResource;
	float spawnCost;
	int enemyClass;
	float spawnRepeatProbability;
	float spawnMissProbability;
	float spawnCostCollateralMultiplier;

};


#endif // ENEMYGENERATOR