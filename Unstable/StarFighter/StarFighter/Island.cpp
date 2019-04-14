#include "Island.h"

extern Game* CurrentGame;

Island::Island(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y) : GameEntity(sf::Vector2f(width * WATERTILE_SIZE, height * WATERTILE_SIZE), UI_WaterTile)
{
	m_upcorner_x = upcorner_x;
	m_upcorner_y = upcorner_y;
	m_width = width;
	m_height = height;
	m_seaport = NULL;

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

	//Adding a port
	AddSeaport(Seaport_Small);

	//Add scenariis (choices)
	m_choicesID[0] = 1;
	m_choicesID[1] = 2;
	m_choicesID[2] = -1;
	m_choicesID[3] = -1;

	//UI
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(20);
	m_text.setColor(sf::Color::Black);
	m_text.setString("Turtle Island");
}

Island::~Island()
{
	
}

Seaport* Island::AddSeaport(SeaportType type)
{
	int zone_coord_x = m_tiles.front()->m_zone->m_coord_x;
	int zone_coord_y = m_tiles.front()->m_zone->m_coord_y;

	//find automatically a location around the island to place the seaport
	vector<WaterTile*> candidates;
	for (vector<WaterTile*>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
	{
		int x = (*it)->m_coord_x;
		int y = (*it)->m_coord_x;
		
		//right
		if (x < NB_WATERTILE_X - 1)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x + 1][y];
			if (tile->m_type == Water_Empty)
			{
				candidates.push_back(tile);
			}
		}
		//left
		if (x > 0)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x - 1][y];
			if (tile->m_type == Water_Empty)
			{
				candidates.push_back(tile);
			}
		}
		//up
		if (y < NB_WATERTILE_Y - 1)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y + 1];
			if (tile->m_type == Water_Empty)
			{
				candidates.push_back(tile);
			}
		}
		//down
		if (y > 0)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y - 1];
			if (tile->m_type == Water_Empty)
			{
				candidates.push_back(tile);
			}
		}
	}

	int candidates_size = candidates.size();
	int random = RandomizeIntBetweenValues(0, candidates_size - 1);
	WaterTile* random_tile = candidates[random];

	//create the port
	Seaport* port = new Seaport(random_tile->m_coord_x, random_tile->m_coord_y, zone_coord_x, zone_coord_y, type, this);
	m_seaport = port;
	m_seaport->m_tile->m_seaport = port;

	return port;
}