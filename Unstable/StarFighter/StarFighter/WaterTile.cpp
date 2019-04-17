#include "WaterTile.h"

extern Game* CurrentGame;

WaterTile::WaterTile(int coord_x, int coord_y, WaterTileType type, WaterZone* zone, int zone_coord_x, int zone_coord_y, float size) : GameEntity(sf::Vector2f(size, size), UI_WaterTile)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_position = sf::Vector2f(WATERTILE_OFFSET_X + coord_x * WATERTILE_SIZE, WATERTILE_OFFSET_Y + (NB_WATERTILE_Y - coord_y) * WATERTILE_SIZE);
	m_type = type;
	m_zone = zone;
	m_DMS.SetCoord(zone_coord_x, coord_x, 0, zone_coord_y, coord_y, 0);
	m_default_color = sf::Color(255, 255, 255, 120);
	m_can_be_seen = false;
	m_location = Location_None;

	//pathfinding
	m_heuristic = 0;
	m_movement_cost = 0;
	m_G_value = 0;
	m_parent = NULL;

	//locations
	m_island = NULL;
	m_seaport = NULL;

	//UI
	m_shape_container.setPosition(m_position);
	m_shape_container.setSize(sf::Vector2f(size, size));
	m_shape_container.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Flood]);
	m_shape_container.setOutlineThickness(-1.f);
	m_shape_container.setOutlineColor(m_default_color);

	m_shape.setPosition(sf::Vector2f(m_position));
	m_shape.setSize(sf::Vector2f(size, size));
	m_shape.setOrigin(sf::Vector2f(size * 0.5f, size * 0.5f));
	m_shape.setFillColor(sf::Color(0, 0, 0, 0));

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::White);
	m_text.SetPosition(m_position);
}

WaterTile::~WaterTile()
{
	
}

void WaterTile::UpdatePosition(DMS_Coord warship_DMS)
{
	m_position.x = WATERTILE_OFFSET_X + WATERTILE_SIZE * (0.5f - (warship_DMS.m_minute_x + warship_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE + m_coord_x);
	m_position.y = WATERTILE_OFFSET_Y + WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - warship_DMS.m_minute_y - warship_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE - m_coord_y + NB_WATERTILE_Y);//from bottom to top

	GameEntity::UpdatePosition();
}

DMS_Coord WaterTile::GetDMSCoord(sf::Vector2f position, DMS_Coord warship_DMS)
{
	DMS_Coord dms;
	float f_minute_x;
	float f_minute_y;

	f_minute_x = (position.x - WATERTILE_OFFSET_X - WATERTILE_SIZE * (0.5f - (warship_DMS.m_minute_x + warship_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE)) / WATERTILE_SIZE;
	f_minute_y = (position.y - WATERTILE_OFFSET_Y - WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - warship_DMS.m_minute_y - warship_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE + NB_WATERTILE_Y)) / (-WATERTILE_SIZE);

	int minute_x = (int)f_minute_x;
	int minute_y = (int)f_minute_y;

	while (minute_y < 0)
	{
		minute_y--;

	}

	float second_x = (f_minute_x - minute_x) * NB_WATERTILE_SUBDIVISION;
	float second_y = (f_minute_y - minute_y) * NB_WATERTILE_SUBDIVISION;

	dms = { 0, minute_x, second_x, 0, minute_y, second_y };
	return dms;
}

bool WaterTile::SameDMS(DMS_Coord DMS_a, DMS_Coord DMS_b)
{
	return DMS_a.m_degree_x == DMS_b.m_degree_x && DMS_a.m_degree_y == DMS_b.m_degree_y && DMS_a.m_minute_x == DMS_b.m_minute_x && DMS_a.m_minute_y == DMS_b.m_minute_y && DMS_a.m_second_x == DMS_b.m_second_x && DMS_a.m_second_y == DMS_b.m_second_y;
}