#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//score display
	score_blue_text.setFont(*(*CurrentGame).font);
	score_blue_text.setCharacterSize(30);
	score_blue_text.setColor(sf::Color::Blue);
	score_blue_text.setPosition((*CurrentGame).map_size.x / 2 + 50, 10);

	score_red_text.setFont(*(*CurrentGame).font);
	score_red_text.setCharacterSize(30);
	score_red_text.setColor(sf::Color::Red);
	score_red_text.setPosition((*CurrentGame).map_size.x / 2 - 50, 10);

	ostringstream ss_score_blue, ss_score_red;
	ss_score_blue << (*CurrentGame).score_blue_team;
	ss_score_red << (*CurrentGame).score_red_team;
	score_blue_text.setString(ss_score_blue.str());
	score_red_text.setString(ss_score_red.str());

	(*CurrentGame).addToFeedbacks(&score_blue_text);
	(*CurrentGame).addToFeedbacks(&score_red_text);
	
	//launch script
	//SetIngameScript(MainMenuScript);
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
	case OfflineMultiContinue:
		StartMultiGame(false);
		break;
	case Shooting01:
		StartShooting01();
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

Bumper* InGameState::CreateBumper(BumperType type, sf::Vector2f position, bool vertical, float size)
{
	sf::Vector2f size_ = vertical ? sf::Vector2f(BUMPER_WIDTH, size) : sf::Vector2f(size, BUMPER_WIDTH);
	Bumper* bumper = new Bumper(type, position, size_);
	(*CurrentGame).addToScene(bumper, BumperLayer, bumper->collider_type);

	return bumper;
}

GameObject* InGameState::CreateGoal(Teams team, sf::Vector2f position, sf::Vector2f size)
{
	std::string str;
	str.assign(team == BlueTeam ? "Assets/2D/goal_blue.png" : "Assets/2D/goal_red.png");
	GameObject* goal = new GameObject(position, sf::Vector2f(0, 0), str, size);
	GameObjectType collider_type = team == BlueTeam ? GoalBlueObject : GoalRedObject;

	(*CurrentGame).addToScene(goal, GoalLayer, collider_type);

	return goal;
}

LevelPortal* InGameState::CreateLevelPortal(IngameScript script, sf::Vector2f position)
{
	LevelPortal* portal = new LevelPortal(script, position, sf::Vector2f(0, 0), "Assets/2D/portal.png", sf::Vector2f(96, 96), sf::Vector2f(48, 48), 4, 1);
	(*CurrentGame).addToScene(portal, PortalLayer, LevelPortalObject);

	return portal;
}

void InGameState::StartMainMenu()
{
	(*CurrentGame).cur_GameRules = CarryToAbleInputs;

	//intégration placeholder
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f(960, 340), Quorra, RedTeam);
	playerShip2->SetControllerType(AllControlDevices);
	playerShip2->disable_inputs = true;

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/main_menu_background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball();

	CreateLevelPortal(Shooting01, sf::Vector2f(360, 380));
	CreateLevelPortal(MainMenuScript, sf::Vector2f(360, REF_WINDOW_RESOLUTION_Y / 2));
	CreateLevelPortal(MainMenuScript, sf::Vector2f(360, 700));
	CreateLevelPortal(OfflineMulti, sf::Vector2f(1560, 380));
	CreateLevelPortal(MainMenuScript, sf::Vector2f(1560, REF_WINDOW_RESOLUTION_Y / 2));

	CreateBumper(OnlyPlayersThrough, sf::Vector2f(494, REF_WINDOW_RESOLUTION_Y/2), true, (*CurrentGame).map_size.y);

	CreateBumper(OnlyPlayersThrough, sf::Vector2f(1409, REF_WINDOW_RESOLUTION_Y/2), true, (*CurrentGame).map_size.y);
}

void InGameState::StartMultiGame(bool reset_scores)
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;

	Ship* playerShip1 = CreateCharacter(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y/2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f(1820, REF_WINDOW_RESOLUTION_Y/2), Quorra, RedTeam);
	playerShip2->SetControllerType(JoystickControl2);

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2));
	//GameObject* background = new GameObject(sf::Vector2f(960 * 1.5f, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920 * 1.5f, 1080), sf::Vector2f(960 * 1.5f, REF_WINDOW_RESOLUTION_Y/2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));

	CreateGoal(BlueTeam, sf::Vector2f(8, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, 200));

	CreateGoal(RedTeam, sf::Vector2f((*CurrentGame).map_size.x - 8, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, 200));

	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(SAFE_ZONE_X, REF_WINDOW_RESOLUTION_Y/2), true, (*CurrentGame).map_size.y);

	CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - SAFE_ZONE_X, REF_WINDOW_RESOLUTION_Y/2), true, (*CurrentGame).map_size.y);

	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x / 2, REF_WINDOW_RESOLUTION_Y/2), true, 200);

	if (reset_scores)
	{
		(*CurrentGame).score_blue_team = 0;
		(*CurrentGame).score_red_team = 0;
	}
}

void InGameState::StartShooting01()
{
	(*CurrentGame).cur_GameRules = SoloTraining;

	Ship* playerShip1 = CreateCharacter(sf::Vector2f(200, REF_WINDOW_RESOLUTION_Y/2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2));
	//GameObject* background = new GameObject(sf::Vector2f(960 * 1.5f, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920 * 1.5f, 1080), sf::Vector2f(960 * 1.5f, REF_WINDOW_RESOLUTION_Y/2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball(sf::Vector2f(200, REF_WINDOW_RESOLUTION_Y/2));

	CreateGoal(RedTeam, sf::Vector2f((*CurrentGame).map_size.x - 8, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, 200));

	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 200, 100), true, 200);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 100, 200), false, 200);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 196, 96), true, 200);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 96, 196), false, 200);

	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), true, REF_WINDOW_RESOLUTION_Y);

	CreateLevelPortal(MainMenuScript, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 100, 100));
}

void InGameState::Update(sf::Time deltaTime)
{
	chosen_character = (*CurrentGame).playerShip->m_character;

	ostringstream ss_score_blue, ss_score_red;
	ss_score_blue << (*CurrentGame).score_blue_team;
	ss_score_red << (*CurrentGame).score_red_team;
	score_blue_text.setString(ss_score_blue.str());
	score_red_text.setString(ss_score_red.str());

	if (InputGuy::isUsingDebugCommand())
	{
		SetIngameScript(MainMenuScript);
	}

	if ((*CurrentGame).playerShip->isLaunchingScript)
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