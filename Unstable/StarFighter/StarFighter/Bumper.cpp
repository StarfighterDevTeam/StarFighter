#include "Bumper.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Bumper::Bumper()
{

}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_type = type;
	this->Init();
}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_type = type;
	this->Init();
}

void Bumper::Init()
{
	if (m_type == OnlyBlueTeamThrough)
	{
		collider_type = BumperBlueObject;
	}
	else if (m_type == OnlyRedTeamThrough)
	{
		collider_type = BumperRedObject;
	}
	else//if (m_type == OnlyPlayersThrough)
	{
		collider_type = BumperGreenObject;
	}
}

Bumper::~Bumper()
{
	
}

void Bumper::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}