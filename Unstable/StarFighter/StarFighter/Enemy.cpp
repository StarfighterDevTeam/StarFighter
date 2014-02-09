#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	//damage = ENEMYX_DAMAGE;
	//TODO remove this
	/*if (m_enemy_type == EnemyType::EnemyX)
	{
		armor = ENEMYX_ARMOR;
		shield = shield_max = ENEMYX_SHIELD;
		shield_regen = ENEMYX_SHIELD_REGEN;
		damage = ENEMYX_DAMAGE;
		weapon = new Weapon();
		weapon->fire_direction = Vector2i(0,(int)(-1));
	}

	if (m_enemy_type == EnemyType::EnemyY)
	{
		armor = ENEMYY_ARMOR;
		shield = shield_max = ENEMYY_SHIELD;
		shield_regen = ENEMYY_SHIELD_REGEN;
		damage = ENEMYY_DAMAGE;
		weapon = new Weapon(WeaponType::LaserFast);
		weapon->fire_direction = Vector2i(0,(int)(-1));
	}*/
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

	//autmatic fire
	if(isOnScene)
	{
		weapon->setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
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

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	enemy->weapon = this->weapon->Clone();

	return enemy;
}