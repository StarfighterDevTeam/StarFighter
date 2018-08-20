#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48));
	(*CurrentGame).m_playerShip = playerShip;
	(*CurrentGame).addToScene(playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	if (!Ship::LoadShip(playerShip))
	{
		//or create a new save file
		Ship::SaveShip(playerShip);
	}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	//Loot
	//Loot* qi = new Loot(Loot_BonusMeleeRange, sf::Vector2f(SHIP_START_X + 100, SHIP_START_Y - 200));
	//qi->setColor(sf::Color(0, 255, 0, 255));
	//(*CurrentGame).addToScene(qi, LootObjectLayer, LootObject);

	Loot* spear = new Loot(Loot_WeaponSpear, sf::Vector2f(SHIP_START_X + 500, SHIP_START_Y - 200));
	(*CurrentGame).addToScene(spear, LootObjectLayer, LootObject);

	Loot* shuriken = new Loot(Loot_WeaponShuriken, sf::Vector2f(SHIP_START_X + 200, SHIP_START_Y - 200));
	(*CurrentGame).addToScene(shuriken, LootObjectLayer, LootObject);

	//enemies

	float step = 0;

	Enemy* wufeng = new Enemy(Enemy_Wufeng_Summoner, sf::Vector2f(SHIP_START_X + 1000, SHIP_START_Y + 100));
	(*CurrentGame).addToScene(wufeng, EnemyObjectLayer, EnemyObject);

	Landmark* gong = new Landmark(Landmark_Gong, sf::Vector2f(1100, 200));
	(*CurrentGame).addToScene(gong, LandmarkLayer, LandmarkObject);

	//Map
	GameObject* background = new GameObject(sf::Vector2f(1920 * 0.5, 1080 * 0.5), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(1920 * 0.5, 1080 * 0.5));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = background->m_size;
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);
}

void InGameState::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);

	//Spawn enemies
	//while ((*CurrentGame).GetEnemyRatings() < 6)
	//{
	//	Enemy* wufeng = new Enemy(sf::Vector2f(SHIP_START_X + RandomizeFloatBetweenValues(sf::Vector2f(300, -300)), SHIP_START_Y + RandomizeFloatBetweenValues(sf::Vector2f(300, -300))), Enemy_Wufeng_Katana);
	//	
	//	(*CurrentGame).addToScene(wufeng, EnemyObjectLayer, EnemyObject);
	//}

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

	//HUD : update player dash counter
	int dashBarsSize = m_dash_bars.size();
	for (int i = 0; i < (*CurrentGame).m_playerShip->m_dash_ammo_max; i++)
	{
		if (i >= dashBarsSize)
		{
			m_dash_bars.push_back(InitDashBar(i));
			(*CurrentGame).addToFeedbacks(m_dash_bars[i]);
		}

		if (i < (*CurrentGame).m_playerShip->m_dash_ammo_current)
		{
			m_dash_bars[i]->setFillColor(sf::Color(0, 255, 0, 255));
		}
		else
		{
			m_dash_bars[i]->setFillColor(sf::Color(0, 255, 0, 0));
		}

		m_dash_bars[i]->m_visible = true;
	}

	for (int i = (*CurrentGame).m_playerShip->m_dash_ammo_max; i < dashBarsSize; i++)
	{
		m_dash_bars[i]->m_visible = false;
	}

	this->mainWindow->clear();
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
}

void InGameState::Release()
{
	int dashBarsSize = m_dash_bars.size();
	for (int i = 0; i < dashBarsSize; i++)
	{
		delete m_dash_bars[i];
		m_dash_bars[i] = NULL;
	}
}

SFRectangle* InGameState::InitDashBar(int index)
{
	SFRectangle* rect = new SFRectangle();

	rect->setSize(sf::Vector2f(DASH_BAR_WIDTH, 10));
	rect->setOrigin(rect->getSize() * 0.5f);
	rect->setFillColor(sf::Color(0, 0, 0, 0));
	rect->setOutlineThickness(2);
	rect->setOutlineColor(sf::Color(255, 0, 0, 200));
	rect->setPosition(sf::Vector2f((*CurrentGame).m_view.getCenter().x - (*CurrentGame).m_view.getSize().x / 2 + DASH_BAR_WIDTH / 2 + 30 + index*(DASH_BAR_WIDTH + DASH_BAR_SPACING), (*CurrentGame).m_view.getCenter().y - (*CurrentGame).m_view.getSize().y / 2 + 30));

	return rect;
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * ISO_FACTOR_X * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * ISO_FACTOR_Y * deltaTime.asSeconds()));//3D Iso = movement on Y axis are twice slower on screen due to the perspective

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