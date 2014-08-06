#include "EnemyPool.h"

extern Game* CurrentGame;

EnemyPoolElement::EnemyPoolElement (Enemy* m_enemy, EnemyClass m_enemy_class, PatternType m_movepatternType)
{
	this->enemy = m_enemy;
	this->enemy_class = m_enemy_class;
	this->movepatternType = m_movepatternType;
}


EnemyPool::EnemyPool(sf::Vector2f m_position, int m_nb_lines, int m_nb_rows, float m_xspread, float m_yspread, std::vector<EnemyPoolElement*>* m_enemyCluster)
{
	this->position = m_position;
	this->nb_lines = m_nb_lines;
	this->nb_rows = m_nb_rows;
	this->xspread = m_xspread;
	this->yspread = m_yspread;
	this->enemyCluster = new std::vector<EnemyPoolElement*>;
	this->enemyCluster = m_enemyCluster;

	this->size.x = nb_rows * xspread;
	this->size.y = nb_lines * yspread;
	this->speed = m_enemyCluster->front()->enemy->speed;

	EnemyPool::CreateCluster();
};

void EnemyPool::CreateCluster()
{
	int index = 0;
	for (std::vector<EnemyPoolElement*>::iterator it = enemyCluster->begin() ; it != enemyCluster->end(); ++it)
	{
		if ( (*(*it)).enemy_class != EnemyClass::ENEMYPOOL_VOID)
		{
			Enemy* n = (*it)->enemy->Clone();
			n->setVisible(true);
			n->setPosition(sf::Vector2f(this->position.x + (index % nb_rows)*xspread, this->position.y + (floor(index / nb_rows)-1)*yspread));

			n->enemy_class = (*it)->enemy_class;
			n->enemy_class = EnemyClass::ENEMYPOOL_GAMMA;

			(*CurrentGame).addToScene(n,LayerType::EnemyObjectLayer, IndependantType::EnemyObject);
		}
		index++;
	}
}

sf::Vector2f EnemyPool::getClusterSize()
{
	return size;
}

sf::Vector2f EnemyPool::getClusterPosition()
{
	return position;
}