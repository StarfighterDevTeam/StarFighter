#include "RoomTile.h"

extern Game* CurrentGame;

RoomConnexion::RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool open)
{
	m_tiles = tiles; 
	m_open = open;
	m_locked = false;
	m_UI_type = UI_Connexion;
	m_default_color = sf::Color::Green;

	if (tiles.first->m_coord_x == tiles.second->m_coord_x)//horizontal connexion = vertical door
	{
		m_size = sf::Vector2f(ROOMTILE_SIZE, CONNEXION_WIDTH);
	}
	else//vertical connexion = horizontal door
	{
		m_size = sf::Vector2f(CONNEXION_WIDTH, ROOMTILE_SIZE);
	}

	m_position = sf::Vector2f(0.5f * (tiles.first->m_position.x + tiles.second->m_position.x), 0.5f * (tiles.first->m_position.y + tiles.second->m_position.y));

	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(m_size);
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setFillColor(sf::Color::Green);
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(sf::Color::White);
}

void RoomConnexion::SetLock(bool locked)
{
	m_locked = locked;

	if (locked == false)
	{
		m_shape_container.setFillColor(sf::Color::Green);
		m_default_color = sf::Color::Green;
	}
	else
	{
		m_shape_container.setFillColor(sf::Color::Red);
		m_default_color = sf::Color::Red;
	}
}

void RoomConnexion::Update(Time deltaTime)
{
	GameEntity::Update(deltaTime);
}

RoomTile::RoomTile(int coord_x, int coord_y, Room* room, float size) : GameEntity(sf::Vector2f(size, size))
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_room = room;
	m_crew = NULL;
	m_position = sf::Vector2f(ROOMTILE_OFFSET_X + coord_x * ROOMTILE_SIZE, ROOMTILE_OFFSET_Y + coord_y * ROOMTILE_SIZE);

	//pathfinding
	m_heuristic = 0;
	m_movement_cost = 0;
	m_G_value = 0;
	m_parent = NULL;

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


RoomTile* RoomTile::GetRoomTileAtCoord(int coord_x, int coord_y)
{
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		if ((*it)->m_coord_x == coord_x && (*it)->m_coord_y == coord_y)
		{
			return *it;
		}
	}

	return NULL;
}