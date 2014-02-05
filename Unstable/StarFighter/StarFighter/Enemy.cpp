#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage)  : Independant(position, speed,  textureName, size) 
{
	weapon.fire_direction = Vector2i(0,(int)(-1));
	armor = ENEMYX_ARMOR;
	shield = ENEMYX_SHIELD;
	shield_regen = ENEMYX_SHIELD_REGEN;
	damage = m_damage;
	collider_type = IndependantType::EnemyObject;
}

void Enemy::update(sf::Time deltaTime)
{
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