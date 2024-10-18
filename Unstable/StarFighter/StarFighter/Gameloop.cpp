#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//water tiles
	const int background_size = 640;
	for (int mapTileIndex_x = 0; mapTileIndex_x < NB_MAP_TILES_X; mapTileIndex_x++)
	{
		std::vector<MapTile*> mapTilesIter_x;
		for (int mapTileIndex_y = 0; mapTileIndex_y < NB_MAP_TILES_Y; mapTileIndex_y++)
		{
			MapTile* newTile = new MapTile(mapTileIndex_x, mapTileIndex_y, TileType::Water);
			mapTilesIter_x.push_back(newTile);
			(*CurrentGame).addToScene(newTile, MapTileLayer, BackgroundObject);

			//background
			const int nb_background_tiles = (background_size / MAP_TILE_SIZE);
			if (mapTileIndex_y % (background_size / MAP_TILE_SIZE) == 0)
			{
				GameObject* background = new GameObject(sf::Vector2f(0.5f * background_size - 0.5f * MAP_TILE_SIZE + 1.f * (mapTileIndex_x / nb_background_tiles) * background_size, 0.5f * background_size - 0.5f * MAP_TILE_SIZE + 1.f * (mapTileIndex_y / nb_background_tiles) * background_size), sf::Vector2f(0, 0), "2D/water_640_640.jpg", sf::Vector2u(background_size, background_size));
				(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
				background->m_DontGarbageMe = true;
			}
		}
		(*CurrentGame).m_mapTiles.push_back(mapTilesIter_x);
	}

	//background
	//m_background = new GameObject(sf::Vector2f(512 - 0.5f * MAP_TILE_SIZE, 512 - 0.5f * MAP_TILE_SIZE), sf::Vector2f(0, 0), "2D/water.jpg", sf::Vector2u(1024, 1024));
	//(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	//(*CurrentGame).m_map_size = m_background->m_size;
	//m_background->m_DontGarbageMe = true;

	//player ship
	(*CurrentGame).m_playerShip = new PlayerShip(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "2D/pirate_ship_64_64.png", sf::Vector2u(64, 64));
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShipObject);
}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(const float DTIME)
{
	(*CurrentGame).GetMouseInputs(DTIME);

	(*CurrentGame).updateScene(DTIME);

	UpdateCamera(DTIME);
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(const float DTIME)
{
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
}