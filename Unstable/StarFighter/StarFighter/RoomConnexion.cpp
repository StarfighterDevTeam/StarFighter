#include "RoomConnexion.h"

extern Game* CurrentGame;

RoomConnexion::RoomConnexion(pair<RoomTile*, RoomTile*> tiles, bool locked, Ship* ship) : GameEntity(UI_Connexion)
{
	m_tiles = tiles; 
	m_UI_type = UI_Connexion;
	m_destroyed = false;
	m_ship = ship;

	m_size = sf::Vector2f(ROOMTILE_SIZE - 2.f, CONNEXION_WIDTH);//horizontal connexion = vertical door
	if (tiles.first->m_coord_x != tiles.second->m_coord_x)//vertical connexion = horizontal door
	{
		float a = m_size.y;
		m_size.y = m_size.x;
		m_size.x = a;
	}

	m_position = sf::Vector2f(0.5f * (tiles.first->m_position.x + tiles.second->m_position.x), 0.5f * (tiles.first->m_position.y + tiles.second->m_position.y));

	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(m_size);
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	//m_shape_container.setFillColor(sf::Color::Green);
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(sf::Color::Black);
	m_default_color = sf::Color::Black;

	SetLock(locked);
}

void RoomConnexion::SetLock(bool locked)
{
	if (m_destroyed == true)
	{
		return;
	}

	m_is_locked = locked;

	if (locked == false)
	{
		m_shape_container.setFillColor(sf::Color::Green);
	}
	else
	{
		m_shape_container.setFillColor(sf::Color::Red);
	}
}

void RoomConnexion::Update(Time deltaTime)
{
	GameEntity::Update(deltaTime);
}

void RoomConnexion::Destroy()
{
	m_destroyed = true;
	m_is_locked = false;
	m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_default_color = sf::Color(100, 100, 100, 255);//dark grey
}