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

	int reverse_line = Independant::getDirectionMultiplier((*CurrentGame).direction).x;
	int reverse_row = Independant::getDirectionMultiplier((*CurrentGame).direction).y;

	int nb_rows_ = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2i(nb_rows, nb_lines)).x;
	int nb_lines_ = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2i(nb_lines, nb_rows)).x;

	for (std::vector<EnemyPoolElement*>::iterator it = enemyCluster->begin() ; it != enemyCluster->end(); ++it)
	{
		if ( (*(*it)).enemy_class != EnemyClass::ENEMYPOOL_VOID)
		{
			Enemy* n = (*it)->enemy->Clone();
			n->setVisible(true);
			n->setRotation(Independant::getRotation_for_Direction((*CurrentGame).direction));

			//WIP
			float offset_x_ = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f((index % nb_rows)*reverse_line*xspread, (floor(index / nb_rows) - 1)*reverse_line*yspread)).x;
			float offset_y_ = Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f((index % nb_rows)*reverse_row*xspread, (floor(index / nb_rows) - 1)*reverse_row*yspread)).y;

			//n->setPosition(sf::Vector2f(this->position.x + reverse_line*(index % nb_rows_)*Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(xspread, yspread)).x,
			//	this->position.y + reverse_row*(floor(index / nb_lines_) - 1)*Independant::getSize_for_Direction((*CurrentGame).direction, sf::Vector2f(xspread, yspread)).y));
			n->setPosition(sf::Vector2f(this->position.x + offset_x_, this->position.y + offset_y_));
				
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