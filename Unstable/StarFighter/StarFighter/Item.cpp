#include "Item.h"

extern Game* CurrentGame;

using namespace sf;

Item::Item()
{

}

Item::Item(string textureName, int frameNumber) : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), textureName, sf::Vector2f(ITEM_SIZE, ITEM_SIZE), sf::Vector2f(ITEM_SIZE/2, ITEM_SIZE/2), frameNumber, 1)
{

}

Item::Item(ItemType item_type)
{
	string texture_name = "2D/unkown_item.png";
	int frame_number = 1;
	m_item_type = item_type;

	switch (item_type)
	{
		case ItemGlasses:
		{
			texture_name = "2D/glasses.png";
			frame_number = 1;
			m_stratagem = new Stratagem(3);
			break;
		}
		case ItemWallet:
		{
			texture_name = "2D/wallet.png";
			frame_number = 1;
			m_stratagem = new Stratagem(5);
			break;
		}
	}

	GameObject::Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), texture_name, sf::Vector2f(ITEM_SIZE, ITEM_SIZE), frame_number, 1);
}

Item::~Item()
{
	delete m_stratagem;
}

void Item::update(sf::Time deltaTime)
{
	GameObject::update(deltaTime);
}