#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcodé pour l'instant
	FX_death = m_FX_death;
	hasWeapon = false;
}

void Enemy::update(sf::Time deltaTime)
{

	//sheld regen if not maximum
	if (shield < shield_max)
	{
		shield += shield_regen;
		//canceling over-regen
		if (shield > shield_max)
		{
			shield = shield_max;
		}
	}

	//automatic fire
	if(isOnScene & hasWeapon)
	{
		weapon->weaponOffset = sf::Vector2f((((this->m_size.x)/2) + (weapon->ammunition->m_size.x/2)) ,((this->m_size.y)/2) - (weapon->ammunition->m_size.y/2 *weapon->fire_direction.y)) ;
		weapon->setPosition(this->getPosition().x, this->getPosition().y);
		//weapon->setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
		weapon->Fire(IndependantType::EnemyFire);

		//sheld regen if not maximum
		if (shield < getIndependantShield())
		{
			shield += getIndependantShieldRegen();
			//canceling over-regen
			if (shield > getIndependantShield())
			{
				shield = getIndependantShield();
			}
		}
	}
	Independant::update(deltaTime);
}

Enemy* Enemy::Clone()
{
	sf::Vector2f s = this->speed;

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	((Independant*)enemy)->addMoney(this->getMoney());
	enemy->hasWeapon = this->hasWeapon;
	if (enemy->hasWeapon)
		enemy->weapon = this->weapon->Clone();

	return enemy;
}

Independant* Enemy::death()
{
	FX* myFX = this->FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	return (Independant*)myFX;
}
