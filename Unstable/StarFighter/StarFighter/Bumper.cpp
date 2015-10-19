#include "Bumper.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Bumper::Bumper(float angle)
{
	m_reflection_angle = angle;
}

Bumper::Bumper(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, float angle, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_reflection_angle = angle;
	this->Init();
}

Bumper::Bumper(sf::Vector2f position, std::string textureName, sf::Vector2f size, float angle) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_reflection_angle = angle;
	this->Init();
}

void Bumper::Init()
{
	collider_type = GameObjectType::BumperObject;
}

Bumper::~Bumper()
{
	
}

void Bumper::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}