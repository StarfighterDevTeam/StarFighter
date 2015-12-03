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
	score_blue_text.setPosition((*CurrentGame).map_size.x / 2 - SCORE_DISPLAY_OFFSET, 14);

	score_red_text.setFont(*(*CurrentGame).font);
	score_red_text.setCharacterSize(30);
	score_red_text.setColor(sf::Color::Red);
	score_red_text.setPosition((*CurrentGame).map_size.x / 2 + SCORE_DISPLAY_OFFSET, 14);

	ostringstream ss_score_blue, ss_score_red;
	ss_score_blue << (*CurrentGame).score_blue_team;
	ss_score_red << (*CurrentGame).score_red_team;
	score_blue_text.setString(ss_score_blue.str());
	score_red_text.setString(ss_score_red.str());

	(*CurrentGame).addToFeedbacks(&score_blue_text);
	(*CurrentGame).addToFeedbacks(&score_red_text);

	//timer display
	map_timer_text.setFont(*(*CurrentGame).font);
	map_timer_text.setCharacterSize(30);
	map_timer_text.setColor(sf::Color::White);
	map_timer_text.setPosition((*CurrentGame).map_size.x / 2, 14);
	ostringstream ss_time_display;
	ss_time_display << (*CurrentGame).m_map_clock.getElapsedTime().asMilliseconds() / 1000;
	map_timer_text.setString(ss_time_display.str());

	//(*CurrentGame).addToFeedbacks(&map_timer_text);

	//launch script
	//SetIngameScript(OfflineMulti);
	SetIngameScript(MainMenuScript);
	//SetIngameScript(ScriptTest);
}

void InGameState::SetIngameScript(IngameScript script, bool reset_scores)
{
	m_script = script;
	(*CurrentGame).m_map_clock.restart();

	//clean previous scene
	(*CurrentGame).CleanAllGameObjects();

	switch (m_script)
	{
		case MainMenuScript:
		{
			StartMainMenu();
			break;
		}

		case OfflineMulti:
		{
			StartMultiGame(reset_scores);
			break;
		}

		case OfflineMultiBig:
		{
			StartMultiGameBig(reset_scores);
			break;
		}

		case Tuto01:
		{
			StartTuto01();
			break;
		}

		case Tuto02:
		{
			StartTuto02();
			break;
		}

		case Tuto03:
		{
			StartTuto03();
			break;
		}

		case Tuto04:
		{
			StartTuto04();
			break;
		}

		case Tuto05:
		{
			StartTuto05();
			break;
		}

		case ScriptTest:
		{
			StartTest();
			break;
		}
		case OfflineMulti2vs2:
		{
			StartMultiGame2vs2();
			break;
		}
		default:
		{
			StartMainMenu();
			break;
		}
	}
}

Ship* InGameState::CreateCharacter(sf::Vector2f position, PlayableCharacters character, Teams team)
{
	GameObjectType type;

	Ship* newship = NULL;
	if (character == Quorra)
		newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Natalia)
		newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Katyusha)
		newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Savannah)
		newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else
		return newship;

	//Init some more variables
	newship->m_team = team;
	newship->m_character = character;

	Aura* team_aura = NULL;
	if (team == BlueTeam)
	{
		type = PlayerBlueShip;
		team_aura = new Aura(newship, "Assets/2D/aura_blue.png", sf::Vector2f(72, 72));
	}
	else
	{
		type = PlayerRedShip;
		team_aura = new Aura(newship, "Assets/2D/aura_red.png", sf::Vector2f(72, 72));
	}
	newship->m_team_aura = team_aura;
	(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
	(*CurrentGame).addToScene(newship->m_team_aura, AuraLayer, BackgroundObject);

	return newship;
}

ShipIA* InGameState::CreateIACharacter(sf::Vector2f position, PlayableCharacters character, Teams team, IADifficultyLevel IA_level, bool activated)
{
	GameObjectType type;

	ShipIA* newship = NULL;
	if (character == Quorra)
		newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Natalia)
		newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Katyusha)
		newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else if (character == Savannah)
		newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	else
		return newship;

	//Init some more variables
	newship->m_team = team;
	newship->m_character = character;
	newship->SetIADifficultyLevel(IA_level);
	newship->ActivateIA(activated);

	Aura* team_aura = NULL;
	if (team == BlueTeam)
	{
		type = PlayerBlueShip;
		team_aura = new Aura(newship, "Assets/2D/aura_blue.png", sf::Vector2f(72, 72));
	}
	else
	{
		type = PlayerRedShip;
		team_aura = new Aura(newship, "Assets/2D/aura_red.png", sf::Vector2f(72, 72));
	}
	newship->m_team_aura = team_aura;
	(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
	(*CurrentGame).addToScene(newship->m_team_aura, AuraLayer, BackgroundObject);

	return newship;
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
	Goal* goal = new Goal(team, position, size);

	(*CurrentGame).addToScene(goal, GoalLayer, goal->collider_type);

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
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f(960, 340), Quorra, RedTeam);
	playerShip2->SetControllerType(AllControlDevices);
	playerShip2->m_disable_inputs = true;

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/main_menu_background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	CreateDiscoball();

	CreateLevelPortal(Tuto01, sf::Vector2f(360, 380));
	CreateLevelPortal(MainMenuScript, sf::Vector2f(360, REF_WINDOW_RESOLUTION_Y / 2));
	CreateLevelPortal(ScriptTest, sf::Vector2f(360, 700));
	CreateLevelPortal(OfflineMulti, sf::Vector2f(1560, 380));
	CreateLevelPortal(OfflineMultiBig, sf::Vector2f(1560, REF_WINDOW_RESOLUTION_Y / 2));
	CreateLevelPortal(OfflineMulti2vs2, sf::Vector2f(1560, 700));

	CreateBumper(OnlyPlayersThrough, sf::Vector2f(494, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);

	CreateBumper(OnlyPlayersThrough, sf::Vector2f(1409, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);
}

void InGameState::StartMultiGame(bool reset_scores)
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;
	(*CurrentGame).score_to_win = 10;

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y/2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Ship* playerShip1 = CreateIACharacter(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y / 2), Natalia, BlueTeam, IAHard);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateIACharacter(sf::Vector2f((*CurrentGame).map_size.x - 100, REF_WINDOW_RESOLUTION_Y / 2), Quorra, RedTeam, IAHard);
	playerShip2->SetControllerType(JoystickControl2);

	InitializeMapDesign();

	if (reset_scores)
	{
		(*CurrentGame).score_blue_team = 0;
		(*CurrentGame).score_red_team = 0;
	}
}

void InGameState::StartTest()
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;
	(*CurrentGame).score_to_win = 0;

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Ship* playerShip1 = new Ship(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/natalia_test.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	playerShip1->m_team = BlueTeam;
	playerShip1->m_character = Natalia;
	playerShip1->SetControllerType(AllControlDevices);
	(*CurrentGame).addToScene(playerShip1, PlayerShipLayer, PlayerBlueShip);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	//Ship* playerShip2 = CreateIACharacter(sf::Vector2f(1200, REF_WINDOW_RESOLUTION_Y / 2), Savannah, RedTeam, IAEasy, false);
	Ship* playerShip2 = CreateIACharacter(sf::Vector2f(1800, REF_WINDOW_RESOLUTION_Y / 2), Savannah, RedTeam, IAHard, true);
	playerShip2->SetControllerType(JoystickControl2);

	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(1000, 540), true, 500);
	CreateDiscoball();
	CreateGoal(BlueTeam, sf::Vector2f(8, 540), sf::Vector2f(16, 200));
}

void InGameState::StartMultiGameBig(bool reset_scores)
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;
	(*CurrentGame).score_to_win = -1;

	GameObject* background = new GameObject(sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_big.png", sf::Vector2f(2880, 1080), sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Ship* playerShip1 = CreateCharacter(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y / 2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f((*CurrentGame).map_size.x - 100, REF_WINDOW_RESOLUTION_Y / 2), Savannah, RedTeam);
	playerShip2->SetControllerType(JoystickControl2);

	InitializeMapDesign();

	//A
	//auto va1 = sf::Vector2f(600, 250);
	//float sa = 150;
	//CreateBumper(OnlyBlueTeamThrough, va1, true, sa);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);
	//
	////B
	//auto vb1 = sf::Vector2f(1000, 400);
	//float sb = 150;
	//CreateBumper(OnlyPlayersThrough, vb1, true, sb);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);
	//
	////C
	//auto vc1 = sf::Vector2f((*CurrentGame).map_size.x / 2, 250);
	//float sc = 150;
	//CreateBumper(OnlyPlayersThrough, vc1, true, sc);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(vc1.x, (*CurrentGame).map_size.y - vc1.y), true, sc);
	//
	////B
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, vb1.y), true, sb);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);
	//
	////A
	//CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, va1.y), true, sa);
	//CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);

	if (reset_scores)
	{
		(*CurrentGame).score_blue_team = 0;
		(*CurrentGame).score_red_team = 0;
	}
}

void InGameState::StartMultiGame2vs2(bool reset_scores)
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;
	(*CurrentGame).score_to_win = -1;

	GameObject* background = new GameObject(sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_big.png", sf::Vector2f(2880, 1080), sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Ship* playerShip1 = CreateCharacter(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y / 2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = CreateCharacter(sf::Vector2f((*CurrentGame).map_size.x - 100, REF_WINDOW_RESOLUTION_Y / 2), Savannah, RedTeam);
	playerShip2->SetControllerType(JoystickControl2);

	Ship* playerShip3 = CreateCharacter(sf::Vector2f((*CurrentGame).map_size.x - 300, REF_WINDOW_RESOLUTION_Y / 2), Quorra, RedTeam);
	playerShip3->SetControllerType(JoystickControl3);

	Ship* playerShip4 = CreateCharacter(sf::Vector2f(300, REF_WINDOW_RESOLUTION_Y / 2), Katyusha, BlueTeam);
	playerShip4->SetControllerType(JoystickControl4);

	InitializeMapDesign();

	//A
	//auto va1 = sf::Vector2f(600, 250);
	//float sa = 150;
	//CreateBumper(OnlyBlueTeamThrough, va1, true, sa);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);
	//
	////B
	//auto vb1 = sf::Vector2f(1000, 400);
	//float sb = 150;
	//CreateBumper(OnlyPlayersThrough, vb1, true, sb);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);
	//
	////C
	//auto vc1 = sf::Vector2f((*CurrentGame).map_size.x / 2, 250);
	//float sc = 150;
	//CreateBumper(OnlyPlayersThrough, vc1, true, sc);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(vc1.x, (*CurrentGame).map_size.y - vc1.y), true, sc);
	//
	////B
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, vb1.y), true, sb);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);
	//
	////A
	//CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, va1.y), true, sa);
	//CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);

	if (reset_scores)
	{
		(*CurrentGame).score_blue_team = 0;
		(*CurrentGame).score_red_team = 0;
	}
}

void InGameState::InitializeSoloCharacter()
{
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(100, REF_WINDOW_RESOLUTION_Y / 2), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());
}

void InGameState::StartTuto01()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto02;
	(*CurrentGame).score_blue_team = 0;
	(*CurrentGame).score_red_team = 0;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto01.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	const float xo = 400;
	const float xa = 750;
	const float xb = 1500;
	const float ya = REF_WINDOW_RESOLUTION_Y / 2;
	const float goal_size = 400;

	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xo, ya), true, (*CurrentGame).map_size.y);
	CreateDiscoball(sf::Vector2f(xa, ya));

	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X - xo, ya), true, (*CurrentGame).map_size.y);

	CreateGoal(RedTeam, sf::Vector2f((*CurrentGame).map_size.x - 8, ya), sf::Vector2f(16, goal_size));
	AutoFillGoalBumpers(BorderRight, goal_size, ya);
}

void InGameState::StartTuto02()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto03;
	(*CurrentGame).score_blue_team = 0;
	(*CurrentGame).score_red_team = 0;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto02.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	const float xo = 400;
	const float xa = 750;
	const float xb = 1000;
	const float ya = 200;
	const float xc_ = (*CurrentGame).map_size.x - ((*CurrentGame).map_size.x - xb) / 2;
	const float goal_size = 250;
	const float xd_ = (*CurrentGame).map_size.x - xc_ - goal_size / 2;

	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xo, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);
	LevelPortal* portal_left = CreateLevelPortal(MainMenuScript, sf::Vector2f(xa, ya));
	CreateDiscoball(sf::Vector2f(xa, REF_WINDOW_RESOLUTION_Y / 2));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xb, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);

	LevelPortal* portal_right = CreateLevelPortal(MainMenuScript, sf::Vector2f(xc_, ya));
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xc_, REF_WINDOW_RESOLUTION_Y / 2), false, (*CurrentGame).map_size.x - xb);
	CreateGoal(RedTeam, sf::Vector2f(xc_, REF_WINDOW_RESOLUTION_Y - 8), sf::Vector2f(goal_size, 16));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xc_ + goal_size / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xc_ - goal_size / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xc_ - goal_size /2 - xd_ / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - (xc_ + goal_size / 2));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xc_ + goal_size / 2 + xd_ / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - (xc_ + goal_size / 2));

	portal_left->m_destination = portal_right;
	portal_right->m_destination = portal_left;
}

//void InGameState::StartTuto02()
//{
//	(*CurrentGame).cur_GameRules = SoloTraining;
//	(*CurrentGame).score_to_win = 1;
//	m_next_script = Tuto02;
//
//	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
//	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
//	(*CurrentGame).map_size = background->m_size;
//
//	InitializeSoloCharacter();
//
//	const float xo = 400;
//	const float xa = 600;
//	const float xb = 1200;
//	const float xc = 1500;
//	const float ya = 400;
//	const float yb = REF_WINDOW_RESOLUTION_Y - ya;
//	CreateDiscoball(sf::Vector2f(xo, REF_WINDOW_RESOLUTION_Y / 2 + 200));
//
//	// A
//	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa / 2, ya), false, xa);
//	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa + (xb - xa) / 2, ya), false, xb - xa);
//	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa, ((REF_WINDOW_RESOLUTION_Y - ya) / 2) + ya + 1), true, REF_WINDOW_RESOLUTION_Y - ya - 1);
//
//	//B
//	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xb, ya / 2), true, ya);
//	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa + (xb - xa) / 2, ya + 2), false, xb - xa);
//
//	//C
//	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xc + (REF_WINDOW_RESOLUTION_X - xc) / 2, ya), false, REF_WINDOW_RESOLUTION_X - xc);
//	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xb + (xc - xb) /2, ya), false, xc - xb);
//
//	//CreateIACharacter(sf::Vector2f(xa, ya / 2), Savannah, BlueTeam, IAHard, true);
//	//CreateIACharacter(sf::Vector2f(xc, (REF_WINDOW_RESOLUTION_Y + yb) / 2), Quorra, RedTeam, IAHard, true);
//}

void InGameState::StartTuto03()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto04;
	(*CurrentGame).score_blue_team = 0;
	(*CurrentGame).score_red_team = 0;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto03.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	const float xo = 100;
	const float yo = 3 * REF_WINDOW_RESOLUTION_Y / 4;

	const float x1 = 800;
	const float xa = 1400;
	const float ya = 100;
	const float yb = 400;
	const float goal_size = 300;

	Ship* playerShip1 = CreateCharacter(sf::Vector2f(xo, yo), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	CreateIACharacter(sf::Vector2f(x1, ya), Quorra, RedTeam, IAHard, true);
	CreateDiscoball(sf::Vector2f(xa, ya));
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), false, REF_WINDOW_RESOLUTION_X);

	CreateGoal(BlueTeam, sf::Vector2f(xa, REF_WINDOW_RESOLUTION_Y - 8), sf::Vector2f(goal_size, 16));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((xa - goal_size / 2) / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X), false, xa - goal_size / 2);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(((*CurrentGame).map_size.x + (xa + goal_size / 2)) / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - xa - goal_size / 2);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa - goal_size / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X/2), true, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa + goal_size / 2, REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X/2), true, GOAL_SAFE_ZONE_X);

	CreateGoal(RedTeam, sf::Vector2f(REF_WINDOW_RESOLUTION_X - 8, ((REF_WINDOW_RESOLUTION_Y / 2) + (REF_WINDOW_RESOLUTION_Y - GOAL_SAFE_ZONE_X)) / 2), sf::Vector2f(16, REF_WINDOW_RESOLUTION_Y / 2 - GOAL_SAFE_ZONE_X));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X, REF_WINDOW_RESOLUTION_Y / 2 / 2), true, REF_WINDOW_RESOLUTION_Y / 2);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X / 2, REF_WINDOW_RESOLUTION_Y / 2), false, GOAL_SAFE_ZONE_X);
}

void InGameState::StartTuto04()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto05;
	(*CurrentGame).score_blue_team = 0;
	(*CurrentGame).score_red_team = 0;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto04.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	const float xo = 250;
	const float yo = 200;
	const float y_disc = 400;

	const float xa = 400;
	const float xb = 500;
	const float ya = 800;
	const float yb = 500;
	const float yc = 700;
	const float goal_size = 200;

	const float xc = 800;
	const float xd = 1000;
	const float xe = 1200;

	const float xw = 1300;
	const float xx = 1500;
	const float xy = 1700;
	const float xz = (*CurrentGame).map_size.x;
	const float x_goal_ = (xx + xy) / 2;

	const float yw = 600;
	const float yx = 400;

	const float yz = 200;

	//Start
	Ship* playerShip1 = CreateCharacter(sf::Vector2f(xo, yo), Natalia, BlueTeam);
	playerShip1->SetControllerType(AllControlDevices);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());
	CreateDiscoball(sf::Vector2f(xo, y_disc));

	//Switch1
	CreateGoal(BlueTeam, sf::Vector2f(8, ya), sf::Vector2f(16, goal_size));
	AutoFillGoalBumpers(BorderLeft, goal_size, ya);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa, ya /2), true, ya);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xb, (*CurrentGame).map_size.y - ya / 2), true, ya);

	//Switch2
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa, ya / 2), true, ya);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xc, (*CurrentGame).map_size.y - ya / 2), true, ya);

	CreateBumper(OnlyPlayersThrough, sf::Vector2f((xb + xc) / 2, (*CurrentGame).map_size.y - ya / 2), true, ya);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f((xc + xb) /2, (*CurrentGame).map_size.y - ya), false, xc - xb);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xd, yc/2), true, yc);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f((xe + xd) / 2, yc), false, xe - xd);
	
	ShipIA* bot1 = CreateIACharacter(sf::Vector2f((xc + xb) / 2, yb), Quorra, RedTeam, IAHard, true);
	bot1->ForceDiscoballUncontested();
	bot1->ForceTackleDisabled();

	//Switch3
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xe, yc / 2), true, yc);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(((*CurrentGame).map_size.x + xe) / 2, yc), false, (*CurrentGame).map_size.x - xe);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f((xw + xx) / 2, yw), false, xx - xw);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f((xx + xy) / 2, yx), false, xy - xx);

	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((xx + xy) / 2, yc), false, xy - xx);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f((xy + xz) / 2, yw), false, xz - xy);

	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(xx, ((*CurrentGame).map_size.y + yc) / 2), true, (*CurrentGame).map_size.y - yc);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xw, (yc + yw) / 2), true, yc - yw);
	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xx, (yw + yx) / 2), true, yw - yx);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(xy, (yc + yw) / 2), true, yc - yw);


	//Ending
	CreateGoal(RedTeam, sf::Vector2f(x_goal_, 8), sf::Vector2f(goal_size, 16));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((xe + (x_goal_ - goal_size / 2)) / 2, GOAL_SAFE_ZONE_X), false, x_goal_ - goal_size/2 - xe);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(((*CurrentGame).map_size.x + x_goal_ + goal_size/2)/2, GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - x_goal_ - goal_size / 2);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(x_goal_ - goal_size / 2, GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(x_goal_ + goal_size / 2, GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);

	//CreateBumper(OnlyBlueTeamThrough, sf::Vector2f((xe + xd) / 2, yc), false, xe - xd);
	ShipIA* bot2 = CreateIACharacter(sf::Vector2f(x_goal_, yo), Quorra, RedTeam, IAHard, true);
	bot2->ForceDiscoballUncontested();
	bot2->ForceTackleDisabled();
}

void InGameState::StartTuto05()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = MainMenuScript;
	(*CurrentGame).score_blue_team = 0;
	(*CurrentGame).score_red_team = 0;

	GameObject* background = new GameObject(sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto05.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	Ship* playerShip2 = CreateIACharacter(sf::Vector2f((*CurrentGame).map_size.x - 100, REF_WINDOW_RESOLUTION_Y / 2), Quorra, RedTeam, IAEasy);

	InitializeMapDesign();
}

void InGameState::AutoFillGoalBumpers(ScreenBorder border, float goal_size, float goal_pos)
{
	//only to use when goals are against a screen border, to fill automatically the reste of the border
	if (border == BorderLeft)
	{
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(GOAL_SAFE_ZONE_X, (goal_pos - goal_size / 2) / 2), true, goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(GOAL_SAFE_ZONE_X, ((*CurrentGame).map_size.y + (goal_pos + goal_size / 2)) / 2), true, (*CurrentGame).map_size.y - goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(GOAL_SAFE_ZONE_X / 2, goal_pos - goal_size / 2), false, GOAL_SAFE_ZONE_X);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(GOAL_SAFE_ZONE_X / 2, goal_pos + goal_size / 2), false, GOAL_SAFE_ZONE_X);
	}

	else if (border == BorderRight)
	{
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X, (goal_pos - goal_size / 2) / 2), true, goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X, ((*CurrentGame).map_size.y + (goal_pos + goal_size / 2)) / 2), true, (*CurrentGame).map_size.y - goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X / 2, goal_pos - goal_size / 2), false, GOAL_SAFE_ZONE_X);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - GOAL_SAFE_ZONE_X / 2, goal_pos + goal_size / 2), false, GOAL_SAFE_ZONE_X);
	}

	else if (border == BorderUp)
	{
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((goal_pos - goal_size / 2) / 2, GOAL_SAFE_ZONE_X), false, goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(((*CurrentGame).map_size.x + (goal_pos + goal_size / 2)) / 2, GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(goal_pos + goal_size / 2, GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(goal_pos - goal_size / 2, GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
	}

	else// if (border == BorderDown)
	{
		CreateBumper(OnlyPlayersThrough, sf::Vector2f((goal_pos - goal_size / 2) / 2, (*CurrentGame).map_size.y - GOAL_SAFE_ZONE_X), false, goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(((*CurrentGame).map_size.x + (goal_pos + goal_size / 2)) / 2, (*CurrentGame).map_size.y - GOAL_SAFE_ZONE_X), false, (*CurrentGame).map_size.x - goal_pos - goal_size / 2);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(goal_pos + goal_size / 2, (*CurrentGame).map_size.y - GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
		CreateBumper(OnlyPlayersThrough, sf::Vector2f(goal_pos - goal_size / 2, (*CurrentGame).map_size.y - GOAL_SAFE_ZONE_X / 2), true, GOAL_SAFE_ZONE_X);
	}
}

void InGameState::ShootingTrainingTuto01()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;

	GameObject* background = new GameObject(sf::Vector2f(1125, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto03.png", sf::Vector2f(2250, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(1125, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	const float ya = 300;
	const float yb = REF_WINDOW_RESOLUTION_Y - ya;
	const float xO = 300;
	const float xa = 600;
	const float xb = 500 + xa;
	const float xc = 400 + xb;
	const float xd_ = (*CurrentGame).map_size.x -((*CurrentGame).map_size.x - xc) / 2;
	const float xda = 20;
	const float goal_size = 300;

	CreateDiscoball(sf::Vector2f(xO, REF_WINDOW_RESOLUTION_Y / 2));

	// A
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa / 2, ya), false, xa);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa / 2, yb), false, xa);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa, ((REF_WINDOW_RESOLUTION_Y - ya) / 2) + ya), true, REF_WINDOW_RESOLUTION_Y - ya);

	// B
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa + (xb - xa) / 2, ya), false, xb - xa);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa + (xb - xa) / 2, yb), false, xb - xa);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xb, (REF_WINDOW_RESOLUTION_Y - ya) / 2), true, REF_WINDOW_RESOLUTION_Y - ya);

	// C
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xb + (xc - xb) /2, ya), false, xc - xb);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xb + (xc - xb) / 2, yb), false, xc - xb);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xc, ((REF_WINDOW_RESOLUTION_Y - ya) / 2) + ya), true, REF_WINDOW_RESOLUTION_Y - ya);

	// D
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_, (REF_WINDOW_RESOLUTION_Y - ya) / 2), true, REF_WINDOW_RESOLUTION_Y - ya);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xd_ + xda, (REF_WINDOW_RESOLUTION_Y - ya) / 2), true, REF_WINDOW_RESOLUTION_Y - ya);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xc + (xd_ - xc) /2 + xda / 2, yb), false, xd_ - xc + xda);

	//goal
	CreateGoal(RedTeam, sf::Vector2f(xd_ + 8 + xda, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, goal_size));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_ + xda + GOAL_SAFE_ZONE_X, ((REF_WINDOW_RESOLUTION_Y / 2) - (goal_size / 2)) / 2), true, (REF_WINDOW_RESOLUTION_Y / 2) - goal_size / 2);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_ + xda + GOAL_SAFE_ZONE_X, (yb + (REF_WINDOW_RESOLUTION_Y / 2) + (goal_size / 2)) / 2), true, yb - (REF_WINDOW_RESOLUTION_Y / 2) - (goal_size / 2));
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_ + xda + GOAL_SAFE_ZONE_X / 2, (REF_WINDOW_RESOLUTION_Y / 2) - goal_size / 2), false, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_ + xda + GOAL_SAFE_ZONE_X / 2, (REF_WINDOW_RESOLUTION_Y / 2) + goal_size / 2), false, GOAL_SAFE_ZONE_X);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xd_ + xda + GOAL_SAFE_ZONE_X / 2, yb), false, GOAL_SAFE_ZONE_X);

}

void InGameState::Update(sf::Time deltaTime)
{
	chosen_character = (*CurrentGame).playerShip->m_character;

	(*CurrentGame).updateScene(deltaTime);

	//move camera
	UpdateCamera(deltaTime);

	//score display
	ostringstream ss_score_blue, ss_score_red;
	ss_score_blue << (*CurrentGame).score_blue_team;
	ss_score_red << (*CurrentGame).score_red_team;
	score_blue_text.setString(ss_score_blue.str());
	score_red_text.setString(ss_score_red.str());
	score_blue_text.setPosition((*CurrentGame).view.getCenter().x - SCORE_DISPLAY_OFFSET, 14);
	score_red_text.setPosition((*CurrentGame).view.getCenter().x + SCORE_DISPLAY_OFFSET, 14);

	ostringstream ss_time_display;
	ss_time_display << (*CurrentGame).m_map_clock.getElapsedTime().asMilliseconds() / 1000;
	map_timer_text.setString(ss_time_display.str());

	//Reset script after a goal
	if ((*CurrentGame).m_goal_happened && (*CurrentGame).m_after_goal_clock.getElapsedTime().asSeconds() > DELAY_AFTER_GOAL)
	{
		//resetting flag
		(*CurrentGame).m_goal_happened = false;

		//end game? (blue wins)
		if ((*CurrentGame).score_to_win != 0 && (*CurrentGame).score_blue_team >= (*CurrentGame).score_to_win)
		{
			//Solo = load next script
			if ((*CurrentGame).cur_GameRules == SoloTraining)
			{
				SetIngameScript(m_next_script, true);
			}
			//Multi = go back to main menu
			else
			{
				SetIngameScript(MainMenuScript, true);
			}
		}
		//end game? (red wins)
		else if ((*CurrentGame).score_to_win != 0 && (*CurrentGame).score_red_team >= (*CurrentGame).score_to_win)
		{
			//Solo = load next script
			if ((*CurrentGame).cur_GameRules == SoloTraining)
			{
				SetIngameScript(m_script, true);
			}
			//Multi = go back to main menu
			else
			{
				SetIngameScript(MainMenuScript, true);
			}
		}
		//continue
		else
		{
			if ((*CurrentGame).cur_GameRules != SoloTraining)
			{
				SetIngameScript(m_script, false);
			}
		}
	}

	if (InputGuy::isUsingDebugCommand())
	{
		SetIngameScript(MainMenuScript);
	}

	if ((*CurrentGame).playerShip->m_isLaunchingScript)
	{
		SetIngameScript((*CurrentGame).playerShip->m_script);
	}

	if (InputGuy::isRestartingScript())
	{
		SetIngameScript(m_script, true);
	}

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

void InGameState::InitializeMapDesign()
{
	CreateDiscoball(sf::Vector2f((*CurrentGame).map_size.x / 2, REF_WINDOW_RESOLUTION_Y / 2));

	CreateGoal(BlueTeam, sf::Vector2f(8, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, GOAL_SIZE));

	CreateGoal(RedTeam, sf::Vector2f((*CurrentGame).map_size.x - 8, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(16, GOAL_SIZE));

	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(DEFENDER_SAFE_ZONE_X, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);

	CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - DEFENDER_SAFE_ZONE_X, REF_WINDOW_RESOLUTION_Y / 2), true, (*CurrentGame).map_size.y);

	AutoFillGoalBumpers(BorderLeft, GOAL_SIZE, (*CurrentGame).map_size.y / 2);
	AutoFillGoalBumpers(BorderRight, GOAL_SIZE, (*CurrentGame).map_size.y / 2);
}