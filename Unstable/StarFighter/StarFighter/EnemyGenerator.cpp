#include "EnemyGenerator.h"

EnemyGenerator::EnemyGenerator(float m_clockCost, int m_enemyClass)
{
	this->clockCost = m_clockCost;
	this->enemyClass = m_enemyClass;
	this->currentClock.restart();
}