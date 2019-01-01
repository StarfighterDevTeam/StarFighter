#include "RoomTile.h"

extern Game* CurrentGame;

RoomTile::RoomTile(int coord_x, int coord_y, Room* room, float size) : GameEntity(sf::Vector2f(size, size))
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_room = room;
	m_crew = NULL;
	m_position = sf::Vector2f(ROOMTILE_OFFSET_X + coord_x * ROOMTILE_SIZE, ROOMTILE_OFFSET_Y + coord_y * ROOMTILE_SIZE);

	//UI
	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(size, size));
	m_shape_container.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape_container.setFillColor(sf::Color::Black);
	m_shape_container.setOutlineThickness(2.f);
	m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 120));

	m_shape.setPosition(sf::Vector2f(m_position));
	m_shape.setSize(sf::Vector2f(size, size));
	m_shape.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape.setFillColor(sf::Color(0, 0, 0, 0));
}

RoomTile::~RoomTile()
{
	
}