#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).m_playerShip = playerShip;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	if (!Ship::LoadShip(playerShip))
	{
		//or create a new save file
		Ship::SaveShip(playerShip);
	}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	//(*CurrentGame).m_map_size = background->m_size;
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);

	// PICK POCKETS SPECIFIC
	//Walker* walker1 = new Walker(sf::Vector2f(608, 608), sf::Vector2f(0, 0), "2D/savannah1.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 1, 1);
	//(*CurrentGame).addToScene(walker1, AgentLayer, AgentObject);
	//(*CurrentGame).m_playerShip->m_awareness_map.insert(std::pair<Agent*, float>(walker1, 0));

	//Walker* walker2 = new Walker(sf::Vector2f(672, 672), sf::Vector2f(0, 0), "2D/savannah2.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 1, 1);
	//(*CurrentGame).addToScene(walker2, AgentLayer, AgentObject);
	//(*CurrentGame).m_playerShip->m_awareness_map.insert(std::pair<Agent*, float>(walker2, 0));
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
	case SFPanel_Stratagem:
		{
			playerShip->m_SFTargetPanel = new SFStratagemPanel(sf::Vector2f(SFSTRATAGEM_PANEL_WIDTH, SFSTRATAGEM_PANEL_HEIGHT), SFPanel_Stratagem, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	vector<vector<string> > allConfigs = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allConfigVectorSize = allConfigs.size();
	for (size_t i = 0; i < allConfigVectorSize; i++)
	{
		(*CurrentGame).m_gameObjectsConfig.insert(std::map<string, vector<string> >::value_type(allConfigs[i][0], allConfigs[i]));
	}

	allConfigs.clear();
}