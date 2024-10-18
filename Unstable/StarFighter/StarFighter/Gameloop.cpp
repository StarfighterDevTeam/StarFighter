#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(512, 512), sf::Vector2f(0, 0), "2D/water.jpg", sf::Vector2u(1024, 1024));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).m_map_size = m_background->m_size;
	m_background->m_DontGarbageMe = true;

	//water tiles
	for (int mapTileIndex_x = 0; mapTileIndex_x < NB_MAP_TILES_X; mapTileIndex_x++)
	{
		std::vector<MapTile*> mapTilesIter_x;
		for (int mapTileIndex_y = 0; mapTileIndex_y < NB_MAP_TILES_Y; mapTileIndex_y++)
		{
			MapTile* newTile = new MapTile(mapTileIndex_x, mapTileIndex_y, TileType::Water);
			mapTilesIter_x.push_back(newTile);
			(*CurrentGame).addToScene(newTile, MapTileLayer, BackgroundObject);
		}
		(*CurrentGame).m_mapTiles.push_back(mapTilesIter_x);
	}

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