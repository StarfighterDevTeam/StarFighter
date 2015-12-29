#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	//intégration placeholder
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).SetPlayerShip(playerShip);
	(*CurrentGame).addToScene((*CurrentGame).playerShip, PlayerShipLayer, PlayerShip);

	//Grid construction
	const int grid_width = 30;
	const int grid_height = 40;
	const int tile_width = 64;
	const int tile_height = 64;
	const int W = grid_width * tile_width;
	const int H = grid_height * tile_height;

	GameObject tile = GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/tile.png", sf::Vector2f(tile_width, tile_height), sf::Vector2f(tile_width/2, tile_height/2), 1, 2);
	GameObject* fake_grid[grid_width][grid_height];
	for (int i = 0; i < grid_width; i++)
	{
		for (int j = 0; j < grid_height; j++)
		{
			fake_grid[i][j] = tile.Clone();
			fake_grid[i][j]->setPosition(sf::Vector2f(i*tile_width + tile_width / 2, j*tile_height + tile_height / 2));
			(*CurrentGame).addToScene(fake_grid[i][j], FakeGridLayer, BackgroundObject);
		}
	}

	//Creating a background
	GameObject* background = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(W, H), sf::Vector2f(W/2, H/2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	//HACK
	(*CurrentGame).map_size = background->m_size;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());
	(*CurrentGame).playerShip->SetControllerType(AllControlDevices);
}

void InGameState::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);

	//move camera
	UpdateCamera(deltaTime);

	this->mainWindow->clear();
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).view.move(sf::Vector2f((*CurrentGame).playerShip->speed.x * deltaTime.asSeconds(), (*CurrentGame).playerShip->speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).view.getSize().x / 2;
	const float y = (*CurrentGame).view.getSize().y / 2;
	const float a = (*CurrentGame).playerShip->getPosition().x;
	const float b = (*CurrentGame).playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).view.setCenter(x, (*CurrentGame).view.getCenter().y);
	if (a >(*CurrentGame).map_size.x - x)
		(*CurrentGame).view.setCenter((*CurrentGame).map_size.x - x, (*CurrentGame).view.getCenter().y);
	if (b < y)
		(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, y);
	if (b >(*CurrentGame).map_size.y - y)
		(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, (*CurrentGame).map_size.y - y);
}