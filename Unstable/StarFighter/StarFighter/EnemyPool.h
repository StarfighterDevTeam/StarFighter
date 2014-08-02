#ifndef ENEMYPOOL_H_INCLUDED
#define ENEMYPOOL_H_INCLUDED

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "PatternBobby.h"
#include "Game.h"

struct EnemyPoolElement
{
	EnemyPoolElement(Enemy* m_enemy, EnemyClass m_enemy_class, PatternType m_movepatternType);
	Enemy* enemy;
	EnemyClass enemy_class;
	PatternType movepatternType;
};

struct EnemyPool
{
public:
	EnemyPool(sf::Vector2f m_position, int m_nb_lines, int m_nb_rows, float m_xspread, float m_yspread, std::vector<EnemyPoolElement*>* m_enemyCluster);

	int nb_lines;
	int nb_rows;
	float xspread;
	float yspread;
	

	void CreateCluster();
	sf::Vector2f getClusterSize();
	sf::Vector2f getClusterPosition();
private:
	sf::Vector2f size;
	sf::Vector2f position;
	sf::Vector2f speed;
	std::vector<EnemyPoolElement*>* enemyCluster;

	
};



#endif//ENEMYPOOL_H_INCLUDED