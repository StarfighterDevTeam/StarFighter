#include "EnemyGenerator.h"

EnemyGenerator::EnemyGenerator(float m_spawnCost, int m_enemyClass)
{
	this->spawnCost = m_spawnCost;
	this->enemyClass = m_enemyClass;
	this->currentClock.restart();
	this->spawnResource = 0;
}