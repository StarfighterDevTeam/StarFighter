#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : Independant(position, speed,  textureName, size) 
{
	this->weapon.fire_direction = Vector2i(0,(int)(-1));
	this->armor = 20;
	this->shield = 0;
}

void Enemy::update(sf::Time deltaTime)
{
	if(isOnScene)
	{
		weapon.setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
		this->weapon.Fire(IndependantType::EnemyFire);
	}
	Independant::update(deltaTime);
}