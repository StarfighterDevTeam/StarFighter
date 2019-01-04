#include "Island.h"

extern Game* CurrentGame;

Island::Island(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y) : GameEntity(sf::Vector2f(width * WATERTILE_SIZE, height * WATERTILE_SIZE), UI_WaterTile)
{
	m_upcorner_x = upcorner_x;
	m_upcorner_y = upcorner_y;
	m_width = width;
	m_height = height;

	//associating island tiles to island
	for (int y = upcorner_y; y > upcorner_y - height; y--)//y : from top to bottom
	{
		for (int x = upcorner_x; x < upcorner_x + width; x++)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			tile->m_type = Water_Island;
			tile->m_island = this;
			tile->m_shape_container.setFillColor(sf::Color(201, 180, 95, 255));

			m_tiles.push_back(tile);
		}
	}

	//UI
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::Black);
	m_text.setString("Turtle Island");
}

Island::~Island()
{
	
}