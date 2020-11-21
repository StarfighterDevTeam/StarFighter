#ifndef ENEMYPOOL_H_INCLUDED
#define ENEMYPOOL_H_INCLUDED

#include "Enemy.h"

struct EnemyPoolElement
{
	EnemyPoolElement(Enemy* enemy, EnemyClass enemy_class, PatternType movepatternType);
	Enemy* m_enemy;
	EnemyClass m_enemy_class;
	PatternType m_movepatternType;
};

struct EnemyPool
{
public:
	EnemyPool(sf::Vector2f position, int nb_lines, int nb_rows, float xspread, float yspread, std::vector<EnemyPoolElement*>* enemyCluster);

	int m_nb_lines;
	int m_nb_rows;
	float m_xspread;
	float m_yspread;
	

	void CreateCluster();
	sf::Vector2f getClusterSize();
	sf::Vector2f getClusterPosition();
private:
	sf::Vector2f m_size;
	sf::Vector2f m_position;
	sf::Vector2f m_speed;
	std::vector<EnemyPoolElement*>* m_enemyCluster;
};



#endif//ENEMYPOOL_H_INCLUDED