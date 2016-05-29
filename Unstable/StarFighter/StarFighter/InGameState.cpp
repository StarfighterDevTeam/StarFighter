#include "InGameState.h"

extern Game* CurrentGame;

#define MAP_SIZE			4000

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//Loading scripts
	LoadCSVFiles();
	
	Ship* playerShip = new Ship(sf::Vector2f(MAP_SIZE / 2, MAP_SIZE/2), sf::Vector2f(0, 0), "2D/Cursor.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16), 1, 2);
	//playerShip->m_visible = false;
	(*CurrentGame).m_playerShip = playerShip;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	Miner* miner = new Miner(sf::Vector2f(MAP_SIZE / 2 - 700, MAP_SIZE/2 - 300), sf::Vector2f(0, 0), "2D/Miner1.png", sf::Vector2f(69, 84), sf::Vector2f(34.5, 42), 3);
	(*CurrentGame).addToScene(miner, StarshipLayer, StarshipObject);
	miner->m_fuel_tanks["oil"] = miner->m_fuel_max;
	miner->LoadFuel("oil", 150);

	Planet* planet = new Planet(sf::Vector2f(MAP_SIZE / 2 - 500, MAP_SIZE/2 + 200), sf::Vector2f(0, 0), "2D/Planet1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	(*CurrentGame).addToScene(planet, LocationLayer, LocationObject);
	planet->m_drill_sucess_rates["oil"] = 0.5f;
	planet->m_drill_sucess_rates["deuterium"] = 0.5f;
	planet->Build("refinery");
	planet->Build("refinery");

	OreField* ore_field = new OreField(sf::Vector2f(MAP_SIZE / 2, MAP_SIZE/2), sf::Vector2f(0, 0), "2D/Field1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	(*CurrentGame).addToScene(ore_field, LocationLayer, LocationObject);
	ore_field->m_drill_sucess_rates["iron"] = 0.90f;
	ore_field->m_drill_sucess_rates["silver"] = 0.10f;
	ore_field->m_min_ore_weight = ore_field->GetLightestOreWeight();

	miner->AssignToLocation(planet);
	
	//Load saved file
	if (!Ship::LoadShip(playerShip))
	{
		//or create a new save file
		Ship::SaveShip(playerShip);
	}

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = sf::Vector2f(MAP_SIZE, MAP_SIZE);
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);
}

void InGameState::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);

	//move camera
	UpdateCamera(deltaTime);

	//Create and destroy HUD panels
	//case 1: destroying a panel
	if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).m_playerShip->m_SFTargetPanel)
	{
		DestroySFPanel((*CurrentGame).m_playerShip);
	}
	else if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel != SFPanel_None)
	{
		//case 2: creating a panel
		if (!(*CurrentGame).m_playerShip->m_SFTargetPanel)
		{
			CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
		}
		//case 3: changing panel
		else if ((*CurrentGame).m_playerShip->m_SFTargetPanel->m_panel_type != (*CurrentGame).m_playerShip->m_is_asking_SFPanel)
		{
			DestroySFPanel((*CurrentGame).m_playerShip);
			CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
		}
	}

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
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFTargetPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFTargetPanel);
		delete playerShip->m_SFTargetPanel;
		playerShip->m_SFTargetPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFiles()
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	//ore data
	vector<vector<string> > oreConfig = *(FileLoaderUtils::FileLoader(ORE_CSV_FILE));
	size_t oreVectorSize = oreConfig.size();
	for (size_t i = 0; i < oreVectorSize; i++)
	{
		(*CurrentGame).m_oreConfig.insert(std::map<string, vector<string> >::value_type(oreConfig[i][OreData_Name], oreConfig[i]));
	}
	oreConfig.clear();

	//building data
	vector<vector<string> > buildingConfig = *(FileLoaderUtils::FileLoader(BUILDING_CSV_FILE));
	size_t buildingVectorSize = buildingConfig.size();
	for (size_t i = 0; i < buildingVectorSize; i++)
	{
		(*CurrentGame).m_buildingConfig.insert(std::map<string, vector<string> >::value_type(buildingConfig[i][BuildingData_Name], buildingConfig[i]));
	}
	buildingConfig.clear();
}