#include "Item.h"

extern Game* CurrentGame;

using namespace sf;

Item::Item()
{
	Init();
}

Item::Item(string textureName, int frameNumber) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(ITEM_SIZE, ITEM_SIZE), sf::Vector2f(ITEM_SIZE/2, ITEM_SIZE/2), frameNumber, 1)
{
	Init();
}

void Item::Init()
{
	m_stratagem = NULL;
}

Item::~Item()
{
	delete m_stratagem;
}

void Item::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}