#include "MapTile.h"

extern Game* CurrentGame;

MapTile::MapTile(int coord_x, int coord_y, TileType type) : GameObject(MapTile::MapTileCoordToPosition(sf::Vector2u(coord_x, coord_y)), sf::Vector2f(0, 0), sf::Color(0, 0, 128, 255), sf::Vector2u(MAP_TILE_SIZE, MAP_TILE_SIZE), 2)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_type = type;

	m_DontGarbageMe = true;

	//m_shape_container.setPosition(m_position);
	//m_shape_container.setSize(sf::Vector2f(size, size));
	//m_shape_container.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	//m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Water]);
	//m_shape_container.setOutlineThickness(-1.f);
	//m_shape_container.setOutlineColor(m_default_color);
	//
	//m_shape.setPosition(sf::Vector2f(m_position));
	//m_shape.setSize(sf::Vector2f(size, size));
	//m_shape.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	//m_shape.setFillColor(sf::Color(0, 0, 0, 0));
}


MapTile* MapTile::PositionToMapTile(sf::Vector2f position)
{
	const int tileCoord_x = (position.x + 0.5f * MAP_TILE_SIZE) / MAP_TILE_SIZE;
	const int tileCoord_y = (position.y + 0.5f * MAP_TILE_SIZE) / MAP_TILE_SIZE;
	MapTile* tile = (*CurrentGame).m_mapTiles[tileCoord_x][tileCoord_y];
	return tile;
}

sf::Vector2f MapTile::MapTileToPosition(MapTile* pTile)
{
	const sf::Vector2f position = sf::Vector2f(1.f * MAP_TILE_SIZE * pTile->m_coord_x, 1.f * MAP_TILE_SIZE * pTile->m_coord_y);
	return position;
}

sf::Vector2f MapTile::MapTileCoordToPosition(sf::Vector2u coord)
{
	const sf::Vector2f position = sf::Vector2f(1.f * MAP_TILE_SIZE * coord.x, 1.f * MAP_TILE_SIZE * coord.y);
	return position;
}