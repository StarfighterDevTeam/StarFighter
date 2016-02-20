#include "EnemyPool.h"

extern Game* CurrentGame;

EnemyPoolElement::EnemyPoolElement (Enemy* enemy, EnemyClass enemy_class, PatternType movepatternType)
{
	m_enemy = enemy;
	m_enemy_class = enemy_class;
	m_movepatternType = movepatternType;
}

EnemyPool::EnemyPool(sf::Vector2f position, int nb_lines, int nb_rows, float xspread, float yspread, std::vector<EnemyPoolElement*>* enemyCluster)
{
	m_position = position;
	m_nb_lines = nb_lines;
	m_nb_rows = nb_rows;
	m_xspread = xspread;
	m_yspread = yspread;
	m_enemyCluster = new std::vector<EnemyPoolElement*>;
	m_enemyCluster = enemyCluster;

	m_size.x = nb_rows * xspread;
	m_size.y = nb_lines * yspread;
	m_speed = m_enemyCluster->front()->m_enemy->m_speed;

	EnemyPool::CreateCluster();
};

void EnemyPool::CreateCluster()
{
	int index = 0;

	int reverse_line = GameObject::getDirectionMultiplier((*CurrentGame).m_direction).x;
	int reverse_row = GameObject::getDirectionMultiplier((*CurrentGame).m_direction).y;

	int nb_rows_ = GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2i(m_nb_rows, m_nb_lines)).x;
	int nb_lines_ = GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2i(m_nb_lines, m_nb_rows)).x;

	for (std::vector<EnemyPoolElement*>::iterator it = m_enemyCluster->begin() ; it != m_enemyCluster->end(); ++it)
	{
		if ( (*(*it)).m_enemy_class != EnemyClass::ENEMYPOOL_VOID)
		{
			Enemy* n = (*it)->m_enemy->Clone();
			n->m_visible = true;
			n->setRotation(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));

			//WIP
			float offset_x_ = GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2f((index % m_nb_rows)*reverse_line*m_xspread, (floor(index / m_nb_rows) - 1)*reverse_line*m_yspread)).x;
			float offset_y_ = GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2f((index % m_nb_rows)*reverse_row*m_xspread, (floor(index / m_nb_rows) - 1)*reverse_row*m_yspread)).y;

			//n->setPosition(sf::Vector2f(this->position.x + reverse_line*(index % nb_rows_)*GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2f(xspread, yspread)).x,
			//	this->position.y + reverse_row*(floor(index / nb_lines_) - 1)*GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2f(xspread, yspread)).y));
			n->setPosition(sf::Vector2f(m_position.x + offset_x_, m_position.y + offset_y_));
				
				n->m_enemy_class = (*it)->m_enemy_class;
				//n->enemy_class = EnemyClass::ENEMYPOOL_GAMMA;

			(*CurrentGame).addToScene(n,LayerType::EnemyObjectLayer, GameObjectType::EnemyObject);
		}
		index++;
	}
}

sf::Vector2f EnemyPool::getClusterSize()
{
	return m_size;
}

sf::Vector2f EnemyPool::getClusterPosition()
{
	return m_position;
}