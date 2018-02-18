#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum Block_Type
{
	Block_Subground1_Cannon,
	Block_Subground2_Hublot,
	Block_Ground_Wood,
};


class Block : public GameObject
{
public :
	Block(Block_Type block_type, sf::Vector2f position);
	void update(sf::Time deltaTime) override;

	Block_Type m_block_type;
};

#endif // BLOCK_H_INCLUDED
