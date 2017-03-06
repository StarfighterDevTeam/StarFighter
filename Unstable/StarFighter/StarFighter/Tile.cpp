#include "Tile.h"

using namespace sf;

Tile::Tile()
{
	Init();
}

void Tile::Init()
{
	m_collider_type = BackgroundObject;

	m_type = Tile_Street;
	m_heuristic = 0;
	m_movement_cost = 0;
	m_G_value = 0;
	m_parent = 0;

	void FindShortedPath(size_t target_index);
}

Tile::Tile(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, speed, textureName, size, origin, frameNumber, animationNumber)
{
	this->Init();
}

Tile::Tile(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size) : GameObject(position, speed, textureName, size)
{
	this->Init();
}

Tile::~Tile()
{
	
}