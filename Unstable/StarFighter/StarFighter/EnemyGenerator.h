#ifndef ENEMYGENERATOR_H_INCLUDED
#define ENEMYGENERATOR_H_INCLUDED

#include <SFML/Graphics.hpp>

class EnemyGenerator
{
public:
	EnemyGenerator::EnemyGenerator(float m_spawnCost, int m_enemyClass);
	sf::Clock currentClock;
	float spawnResource;
	float spawnCost;
	int enemyClass;
};


#endif // ENEMYGENERATOR