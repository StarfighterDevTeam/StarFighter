#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hp = 2;
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