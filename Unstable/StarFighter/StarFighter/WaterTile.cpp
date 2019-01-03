#include "WaterTile.h"

extern Game* CurrentGame;

WaterTile::WaterTile(int coord_x, int coord_y, WaterTileType type, WaterZone* zone, int zone_coord_x, int zone_coord_y, float size) : GameEntity(sf::Vector2f(size, size))
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_position = sf::Vector2f(WATERTILE_OFFSET_X + coord_x * WATERTILE_SIZE, WATERTILE_OFFSET_Y + coord_y * WATERTILE_SIZE);
	m_type = type;
	m_zone = zone;
	m_DMS.SetCoord(zone_coord_x, coord_x, 0, zone_coord_y, coord_y, 0);

	//pathfinding
	m_heuristic = 0;
	m_movement_cost = 0;
	m_G_value = 0;
	m_parent = NULL;

	//UI
	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(size, size));
	m_shape_container.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape_container.setFillColor(sf::Color(0, 100, 170, 255));
	m_shape_container.setOutlineThickness(-1.f);
	m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 120));

	m_shape.setPosition(sf::Vector2f(m_position));
	m_shape.setSize(sf::Vector2f(size, size));
	m_shape.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape.setFillColor(sf::Color(0, 0, 0, 0));
}

WaterTile::~WaterTile()
{
	
}