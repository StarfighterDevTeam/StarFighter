#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//Fake grid construction
	const unsigned int grid_width = GRID_WIDTH;
	const unsigned int grid_height = GRID_HEIGHT;
	const unsigned int tile_width = TILE_SIZE;
	const unsigned int tile_height = TILE_SIZE;
	const unsigned int W = grid_width * tile_width;
	const unsigned int H = grid_height * tile_height;

	GameObject* tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/tile.png", sf::Vector2f(tile_width, tile_height), sf::Vector2f(tile_width / 2, tile_height / 2), 1, 2);
	(*CurrentGame).addToScene(tile, FakeGridLayer, BackgroundObject);

	//team alliance 1
	vector<int> v;
	v.push_back(PlayerBlue);
	v.push_back(PlayerBlue2);
	(*CurrentGame).m_teams_vs_alliance_map.push_back(v);
	//team alliance 2
	v.clear();
	v.push_back(PlayerRed);
	v.push_back(PlayerRed2);
	(*CurrentGame).m_teams_vs_alliance_map.push_back(v);
	//team neutral
	v.clear();
	v.push_back(PlayerNeutral);
	(*CurrentGame).m_teams_vs_alliance_map.push_back(v);

	//Colors data
	for (size_t i = 0; i < NBVAL_PlayerTeams; i++)
	{
		switch (i)
		{
			case PlayerBlue:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Blue;
				break;
			}
			case PlayerBlue2:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::White;
				break;
			}
			case PlayerRed:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Red;
				break;
			}
			case PlayerRed2:
			{
				(*CurrentGame).m_team_colors[i] = sf::Color::Black;
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

	//Team markers
	for (size_t i = 0; i < NBVAL_PlayerTeams; i++)
	{
		(*CurrentGame).m_team_markers[i] = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/team_markers.png", sf::Vector2f(128, 128), sf::Vector2f(64, 64), 2, NBVAL_PlayerTeams);
	}

	//Gauges data
	CreateFluxGauges();

	//Fluxors data
	for (int i = 0; i < NBVAL_FluxorType; i++)
	{
		(*CurrentGame).m_fluxors[i] = new Fluxor((FluxorType)i);
	}

	//Modules data
	for (int i = 0; i < NBVAL_ModuleType; i++)
	{
		(*CurrentGame).m_modules[i] = new Module((ModuleType)i);
	}

	//HUD production mask (for local player only)
	for (int v = 0; v < 1 + USE_SPLIT_SCREEN; v++)
	{
		for (int i = 0; i < 10; i++)
		{
			SFRectangle* rect = new SFRectangle(sf::Vector2f(0, 1.0f * i * HUD_PANEL_SIZE_Y / 10), sf::Vector2f(1.0f * HUD_PANEL_SIZE_X, 1.0f * HUD_PANEL_SIZE_Y / 10), sf::Color(0, 0, 0, 150), 0.f, Color::Transparent, PlayerNeutral);
			rect->setOrigin(0, 0);
			(*CurrentGame).m_HUD_productions_mask[v].push_back(rect);
		}
	}
	
	//intégration placeholder
	Ship* playerShip = new Ship(Game::GridToPosition(sf::Vector2u(DEFAULT_TILE_START, DEFAULT_TILE_START)), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), PlayerBlue, 3);
	(*CurrentGame).playerShips[playerShip->m_team] = playerShip;
	playerShip->SetControllerType(AllControlDevices);
	(*CurrentGame).addToScene(playerShip, PlayerShipLayer, PlayerShip);

	Ship* playerShip2 = new Ship(Game::GridToPosition(sf::Vector2u(GRID_WIDTH + 1 - DEFAULT_TILE_START, GRID_WIDTH + 1 - DEFAULT_TILE_START)), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), PlayerRed, 3);
	(*CurrentGame).addToScene(playerShip2, PlayerShipLayer, PlayerShip);
	playerShip2->SetControllerType(KeyboardControl2);
	playerShip2->setColor(sf::Color::Red);
	(*CurrentGame).playerShips[playerShip2->m_team] = playerShip2;

	//Ship* playerShip3 = new Ship(Game::GridToPosition(sf::Vector2u(DEFAULT_TILE_START, GRID_WIDTH + 1 - DEFAULT_TILE_START)), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), PlayerBlue2, 3);
	//(*CurrentGame).addToScene(playerShip3, PlayerShipLayer, PlayerShip);
	//playerShip3->SetControllerType(JoystickControl3);
	//(*CurrentGame).playerShips[playerShip3->m_team] = playerShip3;
	//
	//Ship* playerShip4 = new Ship(Game::GridToPosition(sf::Vector2u(GRID_WIDTH + 1 - DEFAULT_TILE_START, DEFAULT_TILE_START)), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), PlayerRed2, 3);
	//(*CurrentGame).addToScene(playerShip4, PlayerShipLayer, PlayerShip);
	//playerShip4->SetControllerType(JoystickControl4);
	//(*CurrentGame).playerShips[playerShip4->m_team] = playerShip4;

	(*CurrentGame).map_size = sf::Vector2f(W, H);

	if (USE_SPLIT_SCREEN == true)
	{
		(*CurrentGame).viewP1.setCenter((*CurrentGame).playerShips[0]->getPosition());
		(*CurrentGame).viewP2.setCenter((*CurrentGame).playerShips[1]->getPosition());
	}
	else
	{
		(*CurrentGame).view.setCenter((*CurrentGame).playerShips[0]->getPosition());
	}
	
	//HACK PROTO
	Module::CreateModule(sf::Vector2u(DEFAULT_TILE_GENERATOR, DEFAULT_TILE_GENERATOR), ModuleType_Generator, PlayerBlue, true, true, LinkRight);
	Module::CreateModule(sf::Vector2u(GRID_WIDTH + 1 - DEFAULT_TILE_GENERATOR, GRID_HEIGHT + 1 - DEFAULT_TILE_GENERATOR), ModuleType_Generator, PlayerRed, true, true, LinkLeft);
	//Module::CreateModule(sf::Vector2u(DEFAULT_TILE_GENERATOR, GRID_HEIGHT + 1 - DEFAULT_TILE_GENERATOR), ModuleType_Generator, PlayerBlue2, true, true, LinkRight);
	//Module::CreateModule(sf::Vector2u(GRID_WIDTH + 1 - DEFAULT_TILE_GENERATOR, DEFAULT_TILE_GENERATOR), ModuleType_Generator, PlayerRed2, true, true, LinkLeft);

	//Neutral blocks
	for (size_t i = 0; i < GRID_HEIGHT; i++)
	{
		//if (i != GRID_HEIGHT / 2 && i != GRID_HEIGHT / 2 - 1 && i != GRID_HEIGHT / 2 + 1)
		//{
		//	Module::CreateModule(sf::Vector2u(GRID_WIDTH / 2 + 1, i), ModuleType_Barrier, PlayerNeutral, true);
		//}
	}
		
	//Spawning Fluxors
	m_fluxor_spawn_zones.push_back(FluxorSpawnZone(sf::FloatRect(0, 0, W/2, H/2), FLUXOR_MAX_POPULATION / 6));
	m_fluxor_spawn_zones.push_back(FluxorSpawnZone(sf::FloatRect(0, H/2, W/2, H/2), FLUXOR_MAX_POPULATION / 6));
	m_fluxor_spawn_zones.push_back(FluxorSpawnZone(sf::FloatRect(W/2, 0, W/2, H/2), FLUXOR_MAX_POPULATION / 6));
	m_fluxor_spawn_zones.push_back(FluxorSpawnZone(sf::FloatRect(W/2, H/2, W/2, H/2), FLUXOR_MAX_POPULATION / 6));
	//main central zone
	m_fluxor_spawn_zones.push_back(FluxorSpawnZone(sf::FloatRect(W / 2 - CENTRAL_FLUXOR_ZONE_TILE_OFFSET, H / 2 - CENTRAL_FLUXOR_ZONE_TILE_OFFSET, 2 * CENTRAL_FLUXOR_ZONE_TILE_OFFSET, 2 * CENTRAL_FLUXOR_ZONE_TILE_OFFSET), FLUXOR_MAX_POPULATION / 3));
}

void InGameState::Update(sf::Time deltaTime)
{
	//update Fluxors spawn zones
	size_t FluxorSpawnZonesVectorSize = m_fluxor_spawn_zones.size();
	for (size_t i = 0; i < FluxorSpawnZonesVectorSize; i++)
	{
		m_fluxor_spawn_zones[i].update(deltaTime);
	}

	//reset modules childhood status
	size_t ModulesVectorSize = (*CurrentGame).GetSceneGameObjectsTyped(ModuleObject).size();
	for (size_t i = 0; i < ModulesVectorSize; i++)
	{
		if ((*CurrentGame).GetSceneGameObjectsTyped(ModuleObject)[i])
		{
			Module* module = (Module*)(*CurrentGame).GetSceneGameObjectsTyped(ModuleObject)[i];
			module->m_is_connected_to_a_circuit = false;
		}
	}

	//update all scene objects
	(*CurrentGame).updateScene(deltaTime);

	//update feedbacks
	size_t ModulesVectorSizeNow = (*CurrentGame).GetSceneGameObjectsTyped(ModuleObject).size();
	for (size_t i = 0; i < ModulesVectorSizeNow; i++)
	{
		if ((*CurrentGame).GetSceneGameObjectsTyped(ModuleObject)[i])
		{
			Module* module = (Module*)(*CurrentGame).GetSceneGameObjectsTyped(ModuleObject)[i];
			module->UpdateFreeTileFeedbacks();
		}
	}

	//update HUD production masks
	for (int v = 0; v < 1 + USE_SPLIT_SCREEN; v++)
	{
		size_t HUDProductionMasksVectorSize = (*CurrentGame).m_HUD_productions_mask[v].size();
		for (size_t i = 0; i < HUDProductionMasksVectorSize; i++)
		{
			assert((*CurrentGame).m_HUD_productions_mask[v][i] != NULL);
			assert((*CurrentGame).playerShips[v] != NULL);

			//hide if player's flux is sufficient for building
			(*CurrentGame).m_HUD_productions_mask[v][i]->m_visible = !((*CurrentGame).playerShips[v]->m_flux >= (*CurrentGame).m_modules[i]->m_flux_max_under_construction);
		}
	}
	
	//move camera
	UpdateCamera(deltaTime);

	this->mainWindow->clear();
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	(*CurrentGame).drawHud();
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	if (USE_SPLIT_SCREEN == true)
	{
		(*CurrentGame).viewP1.move(sf::Vector2f((*CurrentGame).playerShips[0]->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).playerShips[0]->m_speed.y * deltaTime.asSeconds()));
		(*CurrentGame).viewP2.move(sf::Vector2f((*CurrentGame).playerShips[1]->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).playerShips[1]->m_speed.y * deltaTime.asSeconds()));

		//Map border constraints
		const float a1 = (*CurrentGame).playerShips[0]->getPosition().x;
		const float b1 = (*CurrentGame).playerShips[0]->getPosition().y;
		const float x1 = (*CurrentGame).viewP1.getSize().x / 2;
		const float y1 = (*CurrentGame).viewP1.getSize().y / 2;

		if (a1 < x1)
			(*CurrentGame).viewP1.setCenter(x1, (*CurrentGame).viewP1.getCenter().y);
		if (a1 >(*CurrentGame).map_size.x - x1)
			(*CurrentGame).viewP1.setCenter((*CurrentGame).map_size.x - x1, (*CurrentGame).viewP1.getCenter().y);
		if (b1 < y1)
			(*CurrentGame).viewP1.setCenter((*CurrentGame).viewP1.getCenter().x, y1);
		if (b1 >(*CurrentGame).map_size.y - y1)
			(*CurrentGame).viewP1.setCenter((*CurrentGame).viewP1.getCenter().x, (*CurrentGame).map_size.y - y1);

		const float a2 = (*CurrentGame).playerShips[1]->getPosition().x;
		const float b2 = (*CurrentGame).playerShips[1]->getPosition().y;
		if (a2 < x1)
			(*CurrentGame).viewP2.setCenter(x1, (*CurrentGame).viewP2.getCenter().y);
		if (a2 >(*CurrentGame).map_size.x - x1)
			(*CurrentGame).viewP2.setCenter((*CurrentGame).map_size.x - x1, (*CurrentGame).viewP2.getCenter().y);
		if (b2 < y1)
			(*CurrentGame).viewP2.setCenter((*CurrentGame).viewP2.getCenter().x, y1);
		if (b2 >(*CurrentGame).map_size.y - y1)
			(*CurrentGame).viewP2.setCenter((*CurrentGame).viewP2.getCenter().x, (*CurrentGame).map_size.y - y1);
	}
	else
	{
		(*CurrentGame).view.move(sf::Vector2f((*CurrentGame).playerShips[0]->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).playerShips[0]->m_speed.y * deltaTime.asSeconds()));

		//Map border constraints
		const float x = (*CurrentGame).view.getSize().x / 2;
		const float y = (*CurrentGame).view.getSize().y / 2;
		const float a = (*CurrentGame).playerShips[0]->getPosition().x;
		const float b = (*CurrentGame).playerShips[0]->getPosition().y;
		if (a < x)
			(*CurrentGame).view.setCenter(x, (*CurrentGame).view.getCenter().y);
		if (a >(*CurrentGame).map_size.x - x)
			(*CurrentGame).view.setCenter((*CurrentGame).map_size.x - x, (*CurrentGame).view.getCenter().y);
		if (b < y)
			(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, y);
		if (b >(*CurrentGame).map_size.y - y)
			(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, (*CurrentGame).map_size.y - y);
	}
}

void InGameState::CreateFluxGauges()
{
	for (int i = 0; i < NBVAL_GaugeStyles; i++)
	{
		//i == 0: creating green "flux build" gauge
		//i == 1: creating blue "flux" gauge
		//i == 2: creating red "attack flux" gauge
		if (i == 0)
		{
			sf::Color fill_color = sf::Color(255, 255, 255, 120);
			sf::Color outline_color = sf::Color(255 * (i == GaugeStyle_Red), 255 * (i == GaugeStyle_Green), 255 * (i == GaugeStyle_Blue), 0);
			SFRectangle* rect_container = new SFRectangle(sf::Vector2f(0, 0), sf::Vector2f(FLUX_GAUGE_WIDTH, FLUX_GAUGE_HEIGHT), fill_color, FLUX_GAUGE_THICNKESS, outline_color, PlayerNeutral);

			sf::Color fill_color2 = sf::Color(255 * (i == GaugeStyle_Red), 255 * (i == GaugeStyle_Green), 255 * (i == GaugeStyle_Blue), 70);
			sf::Color outline_color2 = sf::Color(0, 0, 0, 0);
			SFRectangle* rect_gauge = new SFRectangle(sf::Vector2f(0, 0), sf::Vector2f(FLUX_GAUGE_WIDTH, FLUX_GAUGE_HEIGHT), fill_color2, FLUX_GAUGE_THICNKESS, outline_color2, PlayerNeutral);

			SFText* text = new SFText((*CurrentGame).m_fonts[Font_Arial], 20, sf::Color::Green, sf::Vector2f(0, 0), PlayerNeutral);

			SFGauge* flux_gauge = new SFGauge(text->Clone(), rect_gauge->Clone(), rect_container->Clone());
			delete rect_container;
			delete rect_gauge;
			delete text;

			(*CurrentGame).m_flux_gauges[i] = flux_gauge;
		}
	}
}