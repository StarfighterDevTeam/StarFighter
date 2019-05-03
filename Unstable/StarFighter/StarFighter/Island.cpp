#include "Island.h"

extern Game* CurrentGame;

Island::Island(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y, Seaport* seaport) : GameEntity(sf::Vector2f(width * WATERTILE_SIZE, height * WATERTILE_SIZE), UI_WaterTile)
{
	m_upcorner_x = upcorner_x;
	m_upcorner_y = upcorner_y;
	m_width = width;
	m_height = height;
	m_zone_coord_x = zone_coord_x;
	m_zone_coord_y = zone_coord_y;
	m_seaport = NULL;
	m_seaport = seaport;

	//texture
	Texture* texture;
	texture = TextureLoader::getInstance()->loadTexture("2D/island_" + std::to_string(width) + "x" + std::to_string(height) + ".png", width * WATERTILE_SIZE, height * WATERTILE_SIZE);

	//associating island tiles to island
	for (int y = upcorner_y; y > upcorner_y - height; y--)//y : from top to bottom
	{
		for (int x = upcorner_x; x < upcorner_x + width; x++)
		{
			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			tile->m_type = Water_Island;
			//tile->m_shape_container.setFillColor(sf::Color(201, 180, 95, 255));
			
			//the whole island corresponds to one texture, which is used as a spritesheet (1 tile = 1 frame). We're now associating the correct frame to each tile composing the island.
			tile->setAnimation(texture, width, height);
			tile->setAnimationLine(upcorner_y - y);
			tile->setFrame(x - upcorner_x);
			
			m_tiles.push_back(tile);
		}
	}

	//assign island's reference tile
	int mid_x = upcorner_x + (width / 2);
	int mid_y = upcorner_y - (height / 2);
	m_tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[mid_x][mid_y];
}

Island::~Island()
{
	
}
