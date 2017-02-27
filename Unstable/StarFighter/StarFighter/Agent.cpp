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

void Agent::GenerateItems()
{
	int nb_items = RandomizeIntBetweenValues(0, 5);
	for (int i = 0; i < nb_items; i++)
	{
		ItemType item_type = (ItemType)RandomizeIntBetweenValues(0, NB_ItemTypes-1);
		if (item_type != 0)
		{
			Item* new_item = new Item(item_type);
			m_items.push_back(new_item);
		}
	}
}

Stratagem* Agent::GetRandomStratagem(ItemType item_type)
{
	Stratagem* new_stratagem = new Stratagem(item_type);
	return new_stratagem;
}