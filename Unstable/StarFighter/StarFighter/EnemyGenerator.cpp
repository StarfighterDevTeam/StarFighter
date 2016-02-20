#include "EnemyGenerator.h"

EnemyGenerator::EnemyGenerator(float spawnCost, int enemyClass, float spawnRepeatProbability, float spawnMissProbability)
{
	m_spawnCost = spawnCost;
	m_enemyClass = enemyClass;

	if (spawnRepeatProbability + spawnMissProbability > 1)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error in scene config file: repeat probability + miss probability should always be <= 1");
	}
	m_spawnRepeatProbability = spawnRepeatProbability;
	m_spawnMissProbability = spawnMissProbability;

	m_currentClock.restart();
	m_spawnResource = 0;
}