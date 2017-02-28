#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "Game.h"
#include "SFTextPop.h"
#include "Stratagem.h"

class Item : public GameObject
{
public :
	Item();
	Item(ItemType item_type);
	Item(std::string textureName, int frameNumber = 1);
	virtual ~Item();
	void update(sf::Time deltaTime) override;
	Item* Clone();

	Stratagem* m_stratagem;
	ItemType m_item_type;
};

#endif // ITEM_H_INCLUDED
