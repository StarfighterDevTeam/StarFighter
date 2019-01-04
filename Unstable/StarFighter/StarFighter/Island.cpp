#include "Island.h"

extern Game* CurrentGame;

Island::Island(int upcorner_x, int upcorner_y, int width, int height, WaterZone* zone, int zone_coord_x, int zone_coord_y) : WaterTile(upcorner_x, upcorner_y, Water_Island, zone, zone_coord_x, zone_coord_y)
{
	//associating island tiles to island
	for (int y = upcorner_y; y < upcorner_y + height; y++)
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

	//text
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::White);
	m_text.SetPosition(m_position);
	m_text.setString("Island of Death");
}

Island::~Island()
{
	
}