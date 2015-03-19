#ifndef ENEMYGENERATOR_H_INCLUDED
#define ENEMYGENERATOR_H_INCLUDED

#include <SFML/Graphics.hpp>

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

};


#endif // ENEMYGENERATOR