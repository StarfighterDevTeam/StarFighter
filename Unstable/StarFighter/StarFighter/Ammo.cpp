#include "Ammo.h"

Ammo::Ammo(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int m_damage) : Independant(position, speed,  textureName, size) 
{
	this->armor = 1;
	this->shield = 0;
	this->shield_regen = 0;
	this->damage = m_damage;
}

/*
void Ammo::update(sf::Time deltaTime)
{
	if(isOnScene)
	{
		weapon.setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
		this->weapon.Fire(IndependantType::EnemyFire);
	}
	Independant::update(deltaTime);
}
*/