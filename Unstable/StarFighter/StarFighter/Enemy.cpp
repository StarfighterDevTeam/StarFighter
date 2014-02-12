#include "Enemy.h"


Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	startPattern = false;
}

void Enemy::update(sf::Time deltaTime)
{
	//patterns
	//static float rad = deltaTime.asMilliseconds();
	//rad += 1/LOOP_SAMPLING_MS*PI;
	//printf ("rad = %f\n", rad);

	
	//begin pattern
	static float r = 50;
	static float angle_rad;
	
	if (!startPattern && this->getPosition().y>300)
	{
		startPattern = true;
		
		speed.x = 0;
		speed.y = 0;
		originPattern.x = getPosition().x;
		originPattern.y = getPosition().y;
	}
	if (startPattern)
	{
		static sf::Clock polarClock;
		
		//papillon !
		float posX= originPattern.x + r*sin(angle_rad)*2;
		float posY= originPattern.y + r*sin(angle_rad)*cos(angle_rad);
		this->setPosition(posX, posY);
		angle_rad = polarClock.getElapsedTime().asSeconds()*M_PI;
	
		printf("polarClock= %f | angle rad= %f  x= %f | y =%f\n", polarClock.getElapsedTime().asSeconds(), angle_rad, this->getPosition().x, this->getPosition().y);

	}

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