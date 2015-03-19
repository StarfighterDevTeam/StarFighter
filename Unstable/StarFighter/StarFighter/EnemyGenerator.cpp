#include "EnemyGenerator.h"
#include <assert.h>

EnemyGenerator::EnemyGenerator(float m_spawnCost, int m_enemyClass, float m_spawnRepeatProbability, float m_spawnMissProbability)
{
	this->spawnCost = m_spawnCost;
	this->enemyClass = m_enemyClass;

	assert(m_spawnRepeatProbability + m_spawnMissProbability <= 1);
	this->spawnRepeatProbability = m_spawnRepeatProbability;
	this->spawnMissProbability = m_spawnMissProbability;

	this->currentClock.restart();
	this->spawnResource = 0;
}