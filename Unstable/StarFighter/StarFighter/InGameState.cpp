#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	//intégration placeholder
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).SetPlayerShip(playerShip);
	playerShip->SetControllerType(AllControlDevices);
	playerShip->m_team = PlayerBlue;
	(*CurrentGame).addToScene(playerShip, PlayerShipLayer, PlayerShip);

	//Fake grid construction
	const unsigned int grid_width = GRID_WIDTH;
	const unsigned int grid_height = GRID_HEIGHT;
	const unsigned int tile_width = TILE_SIZE;
	const unsigned int tile_height = TILE_SIZE;
	const unsigned int W = grid_width * tile_width;
	const unsigned int H = grid_height * tile_height;

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

	//Background
	(*CurrentGame).map_size = sf::Vector2f(W, H);
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());
	
	//HACK PROTO
	Module* module = Module::CreateModule(sf::Vector2u(5, 5), ModuleType_Generator, (*CurrentGame).playerShip->m_team);
	//module->m_flux_autogeneration_time = 0.01f;

	Module* module2 = Module::CreateModule(sf::Vector2u(10, 5), ModuleType_Battery, PlayerRed);
	module2->FinishConstruction();
	module2->m_flux = 15;
	Module* module3 = Module::CreateModule(sf::Vector2u(11, 5), ModuleType_Turret, PlayerRed);
	module3->FinishConstruction();
	module3->m_flux = 1000;

	Module::CreateModule(sf::Vector2u(7, 5), ModuleType_Factory, PlayerBlue);
	Module::CreateModule(sf::Vector2u(8, 5), ModuleType_Accelerator, PlayerBlue);
	Module::CreateModule(sf::Vector2u(6, 5), ModuleType_Amplifier, PlayerBlue);
	Module::CreateModule(sf::Vector2u(5, 6), ModuleType_Relay, PlayerBlue);
	Module::CreateModule(sf::Vector2u(6, 6), ModuleType_Amplifier, PlayerBlue);

	//Spawning Fluxors
	for (int i = 1; i < FLUXOR_MAX_POPULATION; i++)
		Fluxor::CreateFluxor(FluxorType_Green);
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
	(*CurrentGame).view.move(sf::Vector2f((*CurrentGame).playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).playerShip->m_speed.y * deltaTime.asSeconds()));

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