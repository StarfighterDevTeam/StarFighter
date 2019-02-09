#include "RoomTile.h"

extern Game* CurrentGame;

RoomTile::RoomTile(int coord_x, int coord_y, Room* room, float size) : GameEntity(sf::Vector2f(size, size), UI_RoomTile)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_room = room;
	m_crew = NULL;
	m_weapon = NULL;
	m_connexion = NULL;
	m_hull = Hull_None;
	m_pierced = false;
	m_flood = 0;
	m_flooding_timer = 0.f;
	m_rotation_speed = 0.f;
	m_rotation_timer = 0.f;
	for (int i = 0; i < 4; i++)
	{
		m_flood_dir[i] = false;
	}
	m_health_max = ROOMTILE_HEALTH_MAX;
	m_health = m_health_max;

	float offset_x = ROOMTILE_OFFSET_X;
	float offset_y = ROOMTILE_OFFSET_Y;
	m_position = sf::Vector2f(offset_x + coord_x * size, offset_y + coord_y * size);

	//system
	m_system = System_None;
	m_system_tile = NULL;
	m_operator_tile = NULL;
	m_lifebar = NULL;
	m_systembar = NULL;

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
	m_shape_container.setOutlineThickness(-1.f);
	m_shape_container.setOutlineColor(sf::Color(255, 255, 255, 120));
	m_default_color = sf::Color(255, 255, 255, 120);

	//m_shape.setPosition(sf::Vector2f(m_position));
	//m_shape.setSize(sf::Vector2f(size, size));
	//m_shape.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	//m_shape.setFillColor(sf::Color(0, 0, 0, 0));

	//m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_text.setCharacterSize(14);
	//m_text.setColor(sf::Color::Black);
}

RoomTile::~RoomTile()
{
	delete m_lifebar;
	delete m_systembar;
}

RoomTile* RoomTile::GetRoomTileAtCoord(int coord_x, int coord_y, bool is_enemy)
{
	vector<RoomTile*>::iterator begin = is_enemy == false ? (*CurrentGame).m_tiles.begin() : (*CurrentGame).m_enemy_tiles.begin();
	vector<RoomTile*>::iterator end = is_enemy == false ? (*CurrentGame).m_tiles.end() : (*CurrentGame).m_enemy_tiles.end();

	for (vector<RoomTile*>::iterator it = begin; it != end; it++)
	{
		if ((*it)->m_coord_x == coord_x && (*it)->m_coord_y == coord_y)
		{
			return *it;
		}
	}

	return NULL;
}