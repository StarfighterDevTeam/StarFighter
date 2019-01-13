#include "WaterTile.h"

extern Game* CurrentGame;

WaterTile::WaterTile(int coord_x, int coord_y, WaterTileType type, WaterZone* zone, int zone_coord_x, int zone_coord_y, float size) : GameEntity(sf::Vector2f(size, size), UI_WaterTile)
{
	m_coord_x = coord_x;
	m_coord_y = coord_y;
	m_position = sf::Vector2f(0, 0);
	m_type = type;
	m_zone = zone;
	m_DMS.SetCoord(zone_coord_x, coord_x, 0, zone_coord_y, coord_y, 0);
	m_default_color = sf::Color(255, 255, 255, 120);
	m_can_be_seen = false;

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
	m_shape_container.setFillColor(sf::Color(0, 100, 170, 255));
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

void WaterTile::UpdatePositionOnMap(DMS_Coord warship_DMS)
{
	float diff_x = m_coord_x - (warship_DMS.m_minute_x + warship_DMS.m_second_x / NB_WATERTILE_SUBDIVISION);
	float diff_y = m_coord_y - (warship_DMS.m_minute_y + warship_DMS.m_second_y / NB_WATERTILE_SUBDIVISION);

	m_position.x = WARSHIP_MAP_OFFSET_X + WATERTILE_SIZE * diff_x;
	m_position.y = WARSHIP_MAP_OFFSET_Y - WATERTILE_SIZE * diff_y;//from bottom to top

	GameEntity::UpdatePosition();
}

DMS_Coord WaterTile::GetDMSCoord(sf::Vector2f position, DMS_Coord warship_DMS)
{
	DMS_Coord dms;
	float f_minute_x;
	float f_minute_y;

	position.y = 379.7;

	float diff_x = WARSHIP_MAP_OFFSET_X - position.x;
	int diff_minute_x = (int)diff_x / NB_WATERTILE_SUBDIVISION;
	float diff_second_x = diff_x - (diff_minute_x * NB_WATERTILE_SUBDIVISION);
	int minute_x = warship_DMS.m_minute_x - diff_minute_x;
	float second_x = warship_DMS.m_second_x - diff_second_x;
	if (second_x < 0)
	{
		minute_x++;
		second_x += NB_WATERTILE_SUBDIVISION;
	}

	float diff_y = WARSHIP_MAP_OFFSET_Y - position.y;
	int diff_minute_y = (int)diff_y / NB_WATERTILE_SUBDIVISION;
	float diff_second_y = diff_y - (diff_minute_y * NB_WATERTILE_SUBDIVISION);
	int minute_y = warship_DMS.m_minute_y + diff_minute_y;//from bottom to top
	float second_y = warship_DMS.m_second_y + diff_second_y;
	if (second_y < 0)
	{
		minute_y++;
		second_y += NB_WATERTILE_SUBDIVISION;
	}

	dms = { 0, minute_x, second_x, 0, minute_y, second_y };
	return dms;
}

//TACTICAL TILES
TacticalTile::TacticalTile(int coord_x, int coord_y, WaterTileType type, WaterZone* zone, int zone_coord_x, int zone_coord_y, float size) : WaterTile(coord_x, coord_y, type, zone, zone_coord_x, zone_coord_y, size)
{

}

void TacticalTile::UpdateTacticalPositionOnMap()
{
	float delta_pox_x = - NB_TACTILAL_TILES_VIEW_RANGE + m_tactical_posx;
	float delta_pox_y = - NB_TACTILAL_TILES_VIEW_RANGE + m_tactical_posy;
	
	m_position.x = WARSHIP_TACTICALMAP_OFFSET_X + TACTICALTILE_SIZE * (0.5f + delta_pox_x);
	m_position.y = WARSHIP_TACTICALMAP_OFFSET_Y - TACTICALTILE_SIZE * (0.5f + delta_pox_y);
}