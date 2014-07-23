#include "Bot.h"

Bot::Bot(Independant* m_target, Weapon* m_weapon, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : Independant(sf::Vector2f(m_target->getPosition().x, m_target->getPosition().y) , speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcodé pour l'instant
	target = m_target;
	weapon = m_weapon;
	this->visible = true;
	this->isOnScene = true;
	DontGarbageMe = true;
}

void Bot::update(sf::Time deltaTime)
{
	static sf::Vector2f newposition, offset;

	newposition.x = target->getPosition().x;
	newposition.y = target->getPosition().y;

	//call bobbyPattern
	offset = Pattern.GetOffset(deltaTime.asSeconds());
	newposition.x += offset.x;
	newposition.y += offset.y;

	this->setPosition(newposition.x,newposition.y);

	//automatic fire
	if(isOnScene)
	{
		weapon->weaponOffset = sf::Vector2f((((this->m_size.x)/2) + (weapon->ammunition->m_size.x/2)) ,((this->m_size.y)/2) - (weapon->ammunition->m_size.y/2 *weapon->fire_direction.y)) ;
		weapon->setPosition(this->getPosition().x, this->getPosition().y);
		weapon->Fire(IndependantType::FriendlyFire);
	}
	

	AnimatedSprite::update(deltaTime);
}