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
	Item(std::string textureName, int frameNumber = 1);
	void Init();
	virtual ~Item();
	void update(sf::Time deltaTime) override;

	Stratagem* m_stratagem;
};

#endif // ITEM_H_INCLUDED
