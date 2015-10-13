#include "Discoball.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Discoball::Discoball()
{
	
}

Discoball::Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Discoball::Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

void Discoball::Init()
{
	this->collider_type = GameObjectType::DiscoballObject;
}

Discoball::~Discoball()
{
	
}

void Discoball::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}