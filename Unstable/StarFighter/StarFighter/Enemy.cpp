#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hp = 2;

	float angle = RandomizeFloatBetweenValues(sf::Vector2f(0, 360));
	SetSpeedVectorFromAbsoluteSpeedAndAngle(200, angle * M_PI / 180);
}

Enemy::~Enemy()
{
	GameObject* playerShip = (GameObject*)(*CurrentGame).m_playerShip;
	if (playerShip->GetDashEnemy() == this)
	{
		playerShip->SetDashEnemy(NULL);
	}
}


void Enemy::update(sf::Time deltaTime)
{
	//bounce on screen borders
	bool bounced = false;
	if (getPosition().x - m_size.x / 2 < 0 && m_speed.x < 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().x + m_size.x / 2 > REF_WINDOW_RESOLUTION_X  && m_speed.x > 0)
	{
		m_speed.x *= -1;
		bounced = true;
	}
	if (getPosition().y - m_size.y / 2 < 0 && m_speed.y < 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}
	if (getPosition().y + m_size.y / 2 > REF_WINDOW_RESOLUTION_Y && m_speed.y > 0)
	{
		m_speed.y *= -1;
		bounced = true;
	}

	GameObject::update(deltaTime);
}

bool Enemy::DealDamage(int dmg)
{
	m_hp -= dmg;

	if (m_hp <= 0)
	{
		Death();
		return true;
	}

	return false;
}

void Enemy::Death()
{
	m_GarbageMe = true;
}