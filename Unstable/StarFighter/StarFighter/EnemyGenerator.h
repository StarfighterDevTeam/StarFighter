#ifndef ENEMYGENERATOR_H_INCLUDED
#define ENEMYGENERATOR_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <assert.h>
#include "Globals.h"

const float spawnCostCollateralMultiplierTable[NB_HAZARD_LEVELS] = { 0.30, 0.30, 0.30, 0.30, 0.30 };
const float spawnCostMultiplierTable[NB_HAZARD_LEVELS] = { 1.0, 1.5, 2.0, 3.0, 4.0 };

const float enemySpeedModifierTable[NB_HAZARD_LEVELS] = { 1.0, 1.2, 1.5, 1.7, 2.0 };
const float ammoSpeedModifierTable[NB_HAZARD_LEVELS] = { 1.0, 1.1, 1.2, 1.3, 1.5 };

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