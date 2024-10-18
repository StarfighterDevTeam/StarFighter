#ifndef MAPTILE_H_INCLUDED
#define MAPTILE_H_INCLUDED

#include "Globals.h"
#include "Game.h"

enum class TileType : int
{
	Water,
	Land,
	NBVAL
};

class MapTile : public GameObject
{
public :
	MapTile(int coord_x, int coord_y, TileType type);
	~MapTile() {};

	int m_coord_x = 0;
	int m_coord_y = 0;
	TileType m_type = TileType::Water;

	static MapTile* PositionToMapTile(sf::Vector2f position);
	static sf::Vector2f MapTileToPosition(MapTile* pTile);
	static sf::Vector2f MapTileCoordToPosition(sf::Vector2u coord);

private:
	sf::RectangleShape m_rect;
};

#endif // MAPTILE_H_INCLUDED
