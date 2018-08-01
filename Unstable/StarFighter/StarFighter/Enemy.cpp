#include "Enemy.h"

extern Game* CurrentGame;

using namespace sf;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	m_hp = 1;
}

Enemy::~Enemy()
{

}


void Enemy::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}