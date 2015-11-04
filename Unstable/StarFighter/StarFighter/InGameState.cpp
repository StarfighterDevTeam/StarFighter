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

	//launch script
	//SetIngameScript(OfflineMulti);
	SetIngameScript(Tuto03);
}

void InGameState::SetIngameScript(IngameScript script, bool reset_scores)
{
	m_script = script;

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

		default:
		{
			StartMainMenu();
			break;
		}
	}
}

Ship* InGameState::CreateCharacter(sf::Vector2f position, PlayableCharacters character, Teams team, bool IA)
{
	GameObjectType type;
	if (team == BlueTeam)
		type = PlayerBlueShip;
	else
		type = PlayerRedShip;

	if (character == Quorra)
	{
		if (!IA)
		{
			Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
		else
		{
			ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
	}
	if (character == Natalia)
	{
		if (!IA)
		{
			Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
		else
		{
			ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
	}
	if (character == Katyusha)
	{
		if (!IA)
		{
			Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
		else
		{
			ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
	}
	if (character == Savannah)
	{
		if (!IA)
		{
			Ship* newship = new Ship(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
		else
		{
			ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
			newship->m_team = team;
			newship->m_character = character;
			(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
			return newship;
		}
	}

	return NULL;
}

ShipIA* InGameState::CreateIACharacter(sf::Vector2f position, PlayableCharacters character, Teams team, IADifficultyLevel IA_level, bool activated)
{
	GameObjectType type;
	if (team == BlueTeam)
		type = PlayerBlueShip;
	else
		type = PlayerRedShip;

	if (character == Quorra)
	{
		ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		newship->SetIADifficultyLevel(IA_level);
		newship->ActivateIA(activated);
		(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
		return newship;

	}
	if (character == Natalia)
	{
		ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		newship->SetIADifficultyLevel(IA_level);
		newship->ActivateIA(activated);
		(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
		return newship;
	}
	if (character == Katyusha)
	{
		ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/katyusha.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		newship->SetIADifficultyLevel(IA_level);
		newship->ActivateIA(activated);
		(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
		return newship;
	}
	if (character == Savannah)
	{
		ShipIA* newship = new ShipIA(sf::Vector2f(position.x, position.y), sf::Vector2f(0, 0), "Assets/2D/savannah.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
		newship->m_team = team;
		newship->m_character = character;
		newship->SetIADifficultyLevel(IA_level);
		newship->ActivateIA(activated);
		(*CurrentGame).addToScene(newship, PlayerShipLayer, type);
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
	CreateLevelPortal(MainMenuScript, sf::Vector2f(360, 700));
	CreateLevelPortal(OfflineMulti, sf::Vector2f(1560, 380));
	CreateLevelPortal(OfflineMultiBig, sf::Vector2f(1560, REF_WINDOW_RESOLUTION_Y / 2));
	CreateLevelPortal(MainMenuScript, sf::Vector2f(1560, 700));

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
	//playerShip2->SetControllerType(JoystickControl2);

	InitializeMapDesign();

	if (reset_scores)
	{
		(*CurrentGame).score_blue_team = 0;
		(*CurrentGame).score_red_team = 0;
	}
}

void InGameState::StartMultiGameBig(bool reset_scores)
{
	(*CurrentGame).cur_GameRules = ClassicMatchGamesRules;
	(*CurrentGame).score_to_win = 10;

	GameObject* background = new GameObject(sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(2880, 1080), sf::Vector2f(1440, REF_WINDOW_RESOLUTION_Y / 2));
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
	auto va1 = sf::Vector2f(600, 250);
	float sa = 150;
	CreateBumper(OnlyBlueTeamThrough, va1, true, sa);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);

	//B
	auto vb1 = sf::Vector2f(1000, 400);
	float sb = 150;
	CreateBumper(OnlyPlayersThrough, vb1, true, sb);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);

	//C
	auto vc1 = sf::Vector2f((*CurrentGame).map_size.x / 2, 250);
	float sc = 150;
	CreateBumper(OnlyPlayersThrough, vc1, true, sc);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(vc1.x, (*CurrentGame).map_size.y - vc1.y), true, sc);

	//B
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, vb1.y), true, sb);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - vb1.x, (*CurrentGame).map_size.y - vb1.y), true, sb);

	//A
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, va1.y), true, sa);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f((*CurrentGame).map_size.x - va1.x, (*CurrentGame).map_size.y - va1.y), true, sa);

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

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background_tuto01.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
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

void InGameState::StartTuto02()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto02;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	const float xo = 400;
	const float xa = 600;
	const float xb = 1200;
	const float xc = 1500;
	const float ya = 400;
	const float yb = REF_WINDOW_RESOLUTION_Y - ya;
	CreateDiscoball(sf::Vector2f(xo, REF_WINDOW_RESOLUTION_Y / 2 + 200));

	// A
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa / 2, ya), false, xa);
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xa + (xb - xa) / 2, ya), false, xb - xa);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa, ((REF_WINDOW_RESOLUTION_Y - ya) / 2) + ya + 1), true, REF_WINDOW_RESOLUTION_Y - ya - 1);

	//B
	CreateBumper(OnlyRedTeamThrough, sf::Vector2f(xb, ya / 2), true, ya);
	//CreateBumper(OnlyPlayersThrough, sf::Vector2f(xa + (xb - xa) / 2, ya + 2), false, xb - xa);

	//C
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xc + (REF_WINDOW_RESOLUTION_X - xc) / 2, ya), false, REF_WINDOW_RESOLUTION_X - xc);
	CreateBumper(OnlyBlueTeamThrough, sf::Vector2f(xb + (xc - xb) /2, ya), false, xc - xb);

	//CreateIACharacter(sf::Vector2f(xa, ya / 2), Savannah, BlueTeam, IAHard, true);
	//CreateIACharacter(sf::Vector2f(xc, (REF_WINDOW_RESOLUTION_Y + yb) / 2), Quorra, RedTeam, IAHard, true);
}

void InGameState::StartTuto03()
{
	(*CurrentGame).cur_GameRules = SoloTraining;
	(*CurrentGame).score_to_win = 1;
	m_next_script = Tuto02;

	GameObject* background = new GameObject(sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(REF_WINDOW_RESOLUTION_X, REF_WINDOW_RESOLUTION_Y), sf::Vector2f(REF_WINDOW_RESOLUTION_X / 2, REF_WINDOW_RESOLUTION_Y / 2));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).map_size = background->m_size;

	InitializeSoloCharacter();

	const float xa = 1000;
	const float ya = 200;
	CreateIACharacter(sf::Vector2f(xa, ya), Quorra, RedTeam, IAHard, true);
	//CreateDiscoball(sf::Vector2f(xa, ya));
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

	ostringstream ss_score_blue, ss_score_red;
	ss_score_blue << (*CurrentGame).score_blue_team;
	ss_score_red << (*CurrentGame).score_red_team;
	score_blue_text.setString(ss_score_blue.str());
	score_red_text.setString(ss_score_red.str());
	score_blue_text.setPosition((*CurrentGame).view.getCenter().x - SCORE_DISPLAY_OFFSET, 14);
	score_red_text.setPosition((*CurrentGame).view.getCenter().x + SCORE_DISPLAY_OFFSET, 14);

	//Reset script after a goal
	if ((*CurrentGame).m_goal_happened && (*CurrentGame).m_after_goal_clock.getElapsedTime().asSeconds() > DELAY_AFTER_GOAL)
	{
		//resetting flag
		(*CurrentGame).m_goal_happened = false;

		//end game? (blue wins)
		if ((*CurrentGame).score_blue_team >= (*CurrentGame).score_to_win)
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
		else if ((*CurrentGame).score_red_team >= (*CurrentGame).score_to_win)
		{
			SetIngameScript(MainMenuScript, true);
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

	//SafeSpace
	float goal_size_y = GOAL_SIZE;
	float safe_space_x = GOAL_SAFE_ZONE_X;
	float ss = ((*CurrentGame).map_size.y - goal_size_y) / 2;
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(safe_space_x, ss / 2), true, ss);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(safe_space_x, (*CurrentGame).map_size.y - ss / 2), true, ss);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(safe_space_x / 2, ss), false, safe_space_x);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f(safe_space_x / 2, (*CurrentGame).map_size.y - ss), false, safe_space_x);

	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - safe_space_x, ss / 2), true, ss);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - safe_space_x, (*CurrentGame).map_size.y - ss / 2), true, ss);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - safe_space_x / 2, ss), false, safe_space_x);
	CreateBumper(OnlyPlayersThrough, sf::Vector2f((*CurrentGame).map_size.x - safe_space_x / 2, (*CurrentGame).map_size.y - ss), false, safe_space_x);
}