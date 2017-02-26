#include "Agent.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------
Agent::Agent()
{
	
}

void Agent::Init()
{
	m_collider_type = AgentObject;
}

Agent::Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Agent::Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

Agent::~Agent()
{
	for (std::vector<Item*>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		delete (*it);
	}
	m_items.clear();
}

void Agent::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}