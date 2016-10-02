#include "InGameState.h"
#include "SFPanelSpecific.h"

void InGameState::Initialize(sf::RenderWindow* window, SEShip* playerShip)
{
	(*CurrentGame).init(window);
	
	(*CurrentGame).m_playerShip = playerShip;
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	if (!playerShip->Load())
	{
		//or create a new save file
		playerShip->Save();
	}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = background->m_size;
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

	CurrentGame->getMainWindow()->clear();
}

void InGameState::Draw()
{
	SEInGameState::Draw();
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, SEShip* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}

	SEInGameState::CreateSFPanel(panel_type, playerShip);
}


void InGameState::DestroySFPanel(SEShip* playerShip)
{
	SEInGameState::DestroySFPanel(playerShip);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	SEInGameState::LoadCSVFile(scenes_file);
}
