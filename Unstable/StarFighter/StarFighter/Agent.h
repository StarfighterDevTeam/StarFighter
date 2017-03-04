#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Item.h"

class Agent : public GameObject
{
public :
	Agent();
	Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Agent(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Agent();
	void update(sf::Time deltaTime) override;

	vector<Item*> m_items;

	void GenerateItems();
	Stratagem* GetRandomStratagem(ItemType item_type);

	void Roaming();
};

#endif // AGENT_H_INCLUDED
