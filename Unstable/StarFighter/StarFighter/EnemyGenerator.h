#ifndef ENEMYGENERATOR_H_INCLUDED
#define ENEMYGENERATOR_H_INCLUDED

#include "EnemyPool.h"

const float spawnCostCollateralMultiplierTable[NB_HAZARD_LEVELS] = { 0.30, 0.30, 0.30, 0.30, 0.30 };
const float spawnCostMultiplierTable[NB_HAZARD_LEVELS] = { 1.0, 1.5, 2.0, 3.0, 4.0 };

const float enemySpeedModifierTable[NB_HAZARD_LEVELS] = { 1.0, 1.2, 1.5, 1.7, 2.0 };
const float ammoSpeedModifierTable[NB_HAZARD_LEVELS] = { 1.0, 1.1, 1.2, 1.3, 1.5 };

class EnemyGenerator
{
public:
	EnemyGenerator(float spawnCost, int enemyClass, float spawnRepeatProbability = 0, float spawnMissProbability = 0);

	sf::Clock m_currentClock;
	float m_spawnResource;
	float m_spawnCost;
	int m_enemyClass;
	float m_spawnRepeatProbability;
	float m_spawnMissProbability;
	float m_spawnCostCollateralMultiplier;
};


#endif // ENEMYGENERATOR