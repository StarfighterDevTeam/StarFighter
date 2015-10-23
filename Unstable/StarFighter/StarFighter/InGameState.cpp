#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	StartMainMenu();
	m_status = MainMenu;

	//StartMultiGame();
	//m_status = OfflineMulti;
}

void InGameState::StartMainMenu()
{
	(*CurrentGame).cur_GameRules = CarryToAbleInputs;

	//intégration placeholder
	Ship* playerShip1 = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	playerShip1->SetControllerType(AllControlDevices);
	(*CurrentGame).addToScene(playerShip1, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = new Ship(sf::Vector2f(960, 340), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	playerShip2->SetControllerType(AllControlDevices);
	playerShip2->disable_inputs = true;
	playerShip2->team = TeamRed;
	(*CurrentGame).addToScene(playerShip2, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	GameObject* background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/main_menu_background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, 540));
	//GameObject* background = new GameObject(sf::Vector2f(960 * 1.5f, 540), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920 * 1.5f, 1080), sf::Vector2f(960 * 1.5f, 540));
	(*CurrentGame).addToScene(background, LayerType::BackgroundLayer, GameObjectType::BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Discoball* discoball = new Discoball(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/discoball.png", sf::Vector2f(50, 50), sf::Vector2f(25, 25), 4, 4);
	(*CurrentGame).addToScene(discoball, LayerType::DiscoballLayer, GameObjectType::DiscoballObject);
}

void InGameState::StartMultiGame()
{
	(*CurrentGame).cur_GameRules = NormalGameRules;

	//intégration placeholder
	Ship* playerShip1 = new Ship(sf::Vector2f(100, 540), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	playerShip1->SetControllerType(KeyboardControl);
	(*CurrentGame).addToScene(playerShip1, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	// #### HACK
	(*CurrentGame).playerShip = playerShip1;
	(*CurrentGame).view.setCenter((*CurrentGame).playerShip->getPosition());

	Ship* playerShip2 = new Ship(sf::Vector2f(1820, 540), sf::Vector2f(0, 0), "Assets/2D/quorra.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32));
	playerShip2->SetControllerType(JoystickControl);
	playerShip2->team = TeamRed;
	(*CurrentGame).addToScene(playerShip2, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	//GameObject* background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, 540));
	GameObject* background = new GameObject(sf::Vector2f(960 * 1.5f, 540), sf::Vector2f(0, 0), "Assets/2D/background_test.png", sf::Vector2f(1920 * 1.5f, 1080), sf::Vector2f(960 * 1.5f, 540));
	(*CurrentGame).addToScene(background, LayerType::BackgroundLayer, GameObjectType::BackgroundObject);

	// ##### HACK
	(*CurrentGame).map_size = background->m_size;

	Discoball* discoball = new Discoball(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/discoball.png", sf::Vector2f(50, 50), sf::Vector2f(25, 25), 4, 4);
	(*CurrentGame).addToScene(discoball, LayerType::DiscoballLayer, GameObjectType::DiscoballObject);

	GameObject* goal_blue = new GameObject(sf::Vector2f(8, 540), sf::Vector2f(0, 0), "Assets/2D/goal_blue.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_blue, LayerType::GoalLayer, GameObjectType::GoalBlueObject);

	GameObject* goal_red = new GameObject(sf::Vector2f((*CurrentGame).map_size.x - 8, 540), sf::Vector2f(0, 0), "Assets/2D/goal_red.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_red, LayerType::GoalLayer, GameObjectType::GoalRedObject);
}

void InGameState::Update(sf::Time deltaTime)
{
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