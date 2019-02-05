#include "RoomConnexion.h"

extern Game* CurrentGame;

RoomConnexion::RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool open, bool minimized, Ship* ship) : GameEntity(UI_Connexion)
{
	m_tiles = tiles; 
	m_open = open;
	m_locked = false;
	m_UI_type = UI_Connexion;
	m_default_color = sf::Color::Green;
	m_destroyed = false;
	m_ship = ship;

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
	if (m_destroyed == true)
	{
		return;
	}

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

void RoomConnexion::Destroy()
{
	m_destroyed = true;
	m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_default_color = sf::Color(100, 100, 100, 255);//dark grey
}