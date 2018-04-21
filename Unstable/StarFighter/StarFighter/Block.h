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

enum Move_Pattern
{
	Move_Pattern_Horizontal,
	Move_Pattern_Vertical,
	Move_Pattern_None,
};

class Block : public GameObject
{
public :
	Block(Block_Type block_type, sf::Vector2f position, Move_Pattern pattern = Move_Pattern_None, float pattern_amplitude = 0.f, float pattern_speed = 0.f);
	void update(sf::Time deltaTime) override;

	Block_Type m_block_type;

	Move_Pattern m_pattern;
	float m_pattern_amplitude;
	float m_pattern_speed;
};

#endif // BLOCK_H_INCLUDED
