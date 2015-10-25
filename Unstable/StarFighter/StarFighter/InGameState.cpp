#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	SetIngameScript(MainMenuScript);
	SetIngameScript(OfflineMulti);
}

void InGameState::SetIngameScript(IngameScript script)
{
	m_script = script;

	//clean previous scene
	(*CurrentGame).CleanAllGameObjects();

	switch (script)
	{
	case MainMenuScript:
		StartMainMenu();
		break;
	case OfflineMulti:
		StartMultiGame();
		break;
	}
}

Ship* InGameState::CreateCharacter(sf::Vector2f position, PlayableCharacters character, Teams team)
{
	if (character == Quorra)
	{
		Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		(*CurrentGame).addToScene(newship, PlayerShipLayer, PlayerShip);
		return newship;
	}
	if (character == Natalia)
	{
		Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		(*CurrentGame).addToScene(newship, PlayerShipLayer, PlayerShip);
		return newship;
	}
	if (character == Katyusha)
	{
		Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		(*CurrentGame).addToScene(newship, PlayerShipLayer, PlayerShip);
		return newship;
	}
	if (character == Savannah)
	{
		Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		(*CurrentGame).addToScene(newship, PlayerShipLayer, PlayerShip);
		return newship;
	}
	
	return NULL;	
}

Discoball* InGameState::CreateDiscoball(sf::Vector2f position)
{
	Discoball* discoball = new Discoball(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/discoball.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 4, 4);
	(*CurrentGame).addToScene(discoball, DiscoballLayer, DiscoballObject);

	return discoball;
}

void InGameState::StartMainMenu()
{
	(*CurrentGame).cur_GameRules = CarryToAbleInputs;

	//intégration placeholder
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(960, 540), Natalia, TeamBlue);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f(960, 340), Quorra, TeamRed);
	playerShip2->SetControllerType(AllControlDevices);
	playerShip2->disable_inputs = true;

	GameObject* background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/main_menu_background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball();

	LevelPortal* training_shoot = new LevelPortal(MainMenuScript, sf::Vector2f(360, 380), sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(training_shoot, GoalLayer, LevelPortalObject);

	LevelPortal* training_pass = new LevelPortal(MainMenuScript, sf::Vector2f(360, 540), sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(training_pass, GoalLayer, LevelPortalObject);

	LevelPortal* training_dribble = new LevelPortal(MainMenuScript, sf::Vector2f(360, 700), sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(training_dribble, GoalLayer, LevelPortalObject);

	LevelPortal* multi_offline = new LevelPortal(OfflineMulti, sf::Vector2f(1560, 380), sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(multi_offline, GoalLayer, LevelPortalObject);

	LevelPortal* multi_online = new LevelPortal(MainMenuScript, sf::Vector2f(1560, 540), sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(multi_online, GoalLayer, LevelPortalObject);

	Bumper* green_bumper_left = new Bumper(OnlyPlayersThrough, sf::Vector2f(494, 540), "Assets/2D/bumper_green.png", sf::Vector2f(2, 1080), sf::Vector2f(1, 540));
	(*CurrentGame).addToScene(green_bumper_left, BumperLayer, BumperGreenObject);

	Bumper* green_bumper_right = new Bumper(OnlyPlayersThrough, sf::Vector2f(1409, 540), "Assets/2D/bumper_green.png", sf::Vector2f(2, 1080), sf::Vector2f(1, 540));
	(*CurrentGame).addToScene(green_bumper_right, BumperLayer, BumperGreenObject);
}

void InGameState::StartMultiGame()
{
	(*CurrentGame).cur_GameRules = NormalGameRules;

	//intégration placeholder
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(100, 540), Natalia, TeamBlue);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f(1820, 540), Quorra, TeamRed);
	playerShip2->SetControllerType(JoystickControl2);

	GameObject* background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, 540));
	//GameObject* background = new GameObject(sf::Vector2f(960 * 1.5f, 540), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920 * 1.5f, 1080), sf::Vector2f(960 * 1.5f, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball();

	GameObject* goal_blue = new GameObject(sf::Vector2f(8, 540), sf::Vector2f(0, 0), "Assets/2D/goal_blue.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_blue, GoalLayer, GoalBlueObject);

	GameObject* goal_red = new GameObject(sf::Vector2f((*CurrentGame).map_size.x - 8, 540), sf::Vector2f(0, 0), "Assets/2D/goal_red.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_red, GoalLayer, GoalRedObject);

	Bumper* blue_bumper = new Bumper(OnlyBlueTeamThrough, sf::Vector2f(SAFE_ZONE_X, 540), "Assets/2D/bumper_blue.png", sf::Vector2f(2, 1080), sf::Vector2f(1, 540));
	(*CurrentGame).addToScene(blue_bumper, BumperLayer, BumperBlueObject);

	Bumper* red_bumper = new Bumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - SAFE_ZONE_X, 540), "Assets/2D/bumper_red.png", sf::Vector2f(2, 1080), sf::Vector2f(1, 540));
	(*CurrentGame).addToScene(red_bumper, BumperLayer, BumperRedObject);
}

void InGameState::Update(sf::Time deltaTime)
{
	chosen_character = (*CurrentGame).playerShip->m_character;

	if (InputGuy::isUsingDebugCommand())
	{
		SetIngameScript(MainMenuScript);
	}

	if ((*CurrentGame).playerShip->isUsingPortal)
	{
		SetIngameScript((*CurrentGame).playerShip->m_script);
	}

	(*CurrentGame).updateScene(deltaTime);

	//move camera
	UpdateCamera(deltaTime);

	this->mainWindow->clear();
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
	if (a > (*CurrentGame).map_size.x - x)
		(*CurrentGame).view.setCenter((*CurrentGame).map_size.x - x, (*CurrentGame).view.getCenter().y);
	if (b < y)
		(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, y);
	if (b > (*CurrentGame).map_size.y - y)
		(*CurrentGame).view.setCenter((*CurrentGame).view.getCenter().x, (*CurrentGame).map_size.y - y);
	
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
}

void InGameState::Release()
{
	//TODO
}