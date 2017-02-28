#include "Police.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Police::Police()
{
	Init();
}

void Police::Init()
{
	Agent::Init();

	m_display_name = "Police";
}

Police::Police(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : Agent(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	Init();
}

Police::Police(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : Agent(position, speed, textureName, size)
{
	Init();
}

Police::~Police()
{
	
}

void Police::update(sf::Time deltaTime)
{
	Agent::update(deltaTime);
}