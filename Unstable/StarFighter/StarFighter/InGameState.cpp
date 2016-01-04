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

	GameObject* tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/tile.png", sf::Vector2f(tile_width, tile_height), sf::Vector2f(tile_width/2, tile_height/2), 1, 2);
	(*CurrentGame).addToScene(tile, FakeGridLayer, BackgroundObject);

	//Background
	(*CurrentGame).map_size = sf::Vector2f(W, H);
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	//Colors data
	for (int i = 0; i < NBVAL_PlayerTeams; i++)
	{
		switch (i)
			{
			case PlayerBlue:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Blue;
				break;
			}
			case PlayerRed:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Red;
				break;
			}
			case PlayerNeutral:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Yellow;
				break;
			}
			default:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::White;
				break;
			}
		}
	}

	//Fluxors data
	for (int i = 0; i < NBVAL_FluxorType; i++)
	{
		(*CurrentGame).m_fluxors[i] = new Fluxor((FluxorType)i);
	}

	//HACK PROTO
	Module* module = Module::CreateModule(sf::Vector2u(5, 5), ModuleType_Generator, (*CurrentGame).playerShip->m_team, true);

	Module* module2 = Module::CreateModule(sf::Vector2u(11, 5), ModuleType_Battery, PlayerRed, true, LinkRight, 15);
	Module* module3 = Module::CreateModule(sf::Vector2u(11, 4), ModuleType_Turret, PlayerRed, true, LinkDown, 1000);
	Module* module4 = Module::CreateModule(sf::Vector2u(12, 4), ModuleType_Generator, PlayerRed, true, LinkLeft);

	Module::CreateModule(sf::Vector2u(15, 5), ModuleType_Barrier, PlayerNeutral, true);

	//Spawning Fluxors
	//for (int i = 1; i < FLUXOR_MAX_POPULATION; i++)
	//	Fluxor::CreateFluxor(FluxorType_Green);
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