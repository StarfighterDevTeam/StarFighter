#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include "GameObject.h"

enum TileType
{
	Tile_Street,
	Tile_Street_1,
	Tile_Street_2,
	Tile_Street_3,
	Tile_StreetAngle_0,
	Tile_StreetAngle_1,
	Tile_StreetAngle_2,
	Tile_StreetAngle_3,
	Tile_StreetAngleExternal_0,
	Tile_StreetAngleExternal_1,
	Tile_StreetAngleExternal_2,
	Tile_StreetAngleExternal_3,
	Tile_Park_0,
	Tile_Road,
	//Tile_RoadVertical,
	Tile_Building,
	Tile_RoadAngle_0,
	Tile_RoadAngle_1,
	Tile_RoadAngle_2,
	Tile_RoadAngle_3,
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
	
	//a* pathfinding
	int m_heuristic;
	int m_movement_cost;
	int m_G_value;
	size_t m_parent;
};

#endif // TILE_H_INCLUDED
