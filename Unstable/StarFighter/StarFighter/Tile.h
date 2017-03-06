#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include "GameObject.h"

enum TileType
{
	Tile_Street,
	Tile_Building,
	NBVAL_TileTtype,
};

class Tile : public GameObject
{
public :
	Tile();
	Tile(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Tile(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Tile();

	TileType m_type;
	size_t m_index;
	
	//a* pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	size_t m_parent;
};

#endif // TILE_H_INCLUDED
