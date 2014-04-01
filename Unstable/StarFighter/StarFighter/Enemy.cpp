#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcodé pour l'instant
	FX_death = m_FX_death;
}

void Enemy::update(sf::Time deltaTime, sf::Vector2f polarOffset[MovePatternType::NBVAL_MovePattern])
{
	if (getPosition().y >200 && !startPattern)//hauteur de déclenchement du pattern hardcodée
	{
		startPattern = true;
	}
	
	//add offset calculated by Game
	if (startPattern)
	{
		//speed.x = 0;
		//speed.y = 0;
		setPosition(getPosition().x + polarOffset[movepattern_type].x, getPosition().y + polarOffset[movepattern_type].y);
		//printf("pouet\n");
	}
	
	Enemy::update(deltaTime);
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

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	enemy->weapon = this->weapon->Clone();

	return enemy;
}

Independant* Enemy::death()
{
	FX* myFX = this->FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	return (Independant*)myFX;
}
