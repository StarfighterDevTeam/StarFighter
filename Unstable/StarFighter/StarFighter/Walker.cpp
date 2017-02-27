#include "Walker.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Walker::Walker()
{
	
}

void Walker::Init()
{
	Agent::Init();

	GenerateItems();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Agent(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Walker::Walker(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Agent(position, speed, textureName, size)
{
	Init();
}

Walker::~Walker()
{
	
}

void Walker::update(sf::Time deltaTime)
{
	Agent::update(deltaTime);
}