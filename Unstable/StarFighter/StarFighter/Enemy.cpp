#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_enemy_type)  : Independant(position, speed,  textureName, size) 
{
	enemy_type = m_enemy_type;
	collider_type = IndependantType::EnemyObject;
	visible = true;
	weapon.fire_direction = Vector2i(0,(int)(-1));
	//damage = ENEMYX_DAMAGE;

	if (m_enemy_type == EnemyType::EnemyX)
	{
		armor = ENEMYX_ARMOR;
		shield = ENEMYX_SHIELD;
		shield_regen = ENEMYX_SHIELD_REGEN;
		damage = ENEMYX_DAMAGE;
	}

	if (m_enemy_type == EnemyType::EnemyY)
	{
		armor = ENEMYY_ARMOR;
		shield = ENEMYY_SHIELD;
		shield_regen = ENEMYY_SHIELD_REGEN;
		damage = ENEMYY_DAMAGE;
	}
}

void Enemy::update(sf::Time deltaTime)
{
	//sheld regen if not maximum
	if (shield < getIndependantShield())
	{
		shield += shield_regen;
		//canceling over-regen
		if (shield > getIndependantShield())
		{
			shield = getIndependantShield();
		}
	}

	//autmatic fire
	if(isOnScene)
	{
		weapon.setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
		this->weapon.Fire(IndependantType::EnemyFire);

		//sheld regen if not maximum
		if (shield < this->getIndependantShield())
		{
			shield += shield_regen; // !!
			//canceling over-regen
			if (shield > getIndependantShield())
			{
				shield = getIndependantShield();
			}
		}
	}
	Independant::update(deltaTime);
}