#include "Block.h"

extern Game* CurrentGame;

using namespace sf;

Block::Block(Block_Type block_type, sf::Vector2f position, Move_Pattern pattern, float pattern_amplitude, float pattern_speed)
{
	m_block_type = block_type;

	m_pattern = pattern;
	m_pattern_amplitude = pattern_amplitude;
	m_pattern_speed = pattern_speed;

	string textureName;
	sf::Vector2f textureSize = sf::Vector2f(-1, -1);
	int frameNumber = 1;
	int animationNumber = 1;

	switch (m_block_type)
	{
		case Block_Subground1_Cannon:
		{
			textureName = "2D/blocks/subground_wood_192x140.png";
			textureSize = sf::Vector2f(192, 140);
			break;
		}
		case Block_Subground2_Hublot:
		{
			textureName = "2D/blocks/subground2_wood_192x110.png";
			textureSize = sf::Vector2f(192, 110);
			break;
		}
		case Block_Ground_Wood:
		{
			textureName = "2D/blocks/ground_wood_192x34.png";
			textureSize = sf::Vector2f(192, 34);
			break;
		}
	}

	assert(textureName != "");
	assert(textureSize.x > 0 && textureSize.y > 0);

	Init(position, sf::Vector2f(0, 0), textureName, textureSize, frameNumber, animationNumber);
	this->setOrigin(textureSize.x / 2, textureSize.y / 2);
}

void Block::update(sf::Time deltaTime)
{
	switch (m_pattern)
	{
		case Move_Pattern_None:
		break;
	
		case Move_Pattern_Horizontal:
		break;
	}

	GameObject::update(deltaTime);
}