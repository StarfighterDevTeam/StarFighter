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
	m_display_name = "Unknown";
	int frame_number = 1;
	m_item_type = item_type;

	switch (item_type)
	{
		case ItemGlasses:
		{
			texture_name = "2D/glasses.png";
			m_display_name = "Glasses";
			frame_number = 1;
			m_stratagem = new Stratagem(3);
			break;
		}
		case ItemWallet:
		{
			texture_name = "2D/wallet.png";
			m_display_name = "Wallet";
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

Item* Item::Clone()
{
	Item* new_item = new Item(this->m_textureName, this->m_frameNumber);
	if (this->m_stratagem)
	{
		new_item->m_stratagem = new Stratagem();
		for (int i = 0; i < MAX_CODES; i++)
		{
			new_item->m_stratagem->m_code[i] = this->m_stratagem->m_code[i];
		}
	}
	
	new_item->m_item_type = this->m_item_type;

	return new_item;
}