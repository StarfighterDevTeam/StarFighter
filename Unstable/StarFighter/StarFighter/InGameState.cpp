#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	//intégration placeholder
	Ship* playerShip = new Ship(sf::Vector2f(100, 540), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).SetPlayerShip(playerShip);
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	GameObject* background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1920, 1080), sf::Vector2f(960, 540));
	(*CurrentGame).addToScene(background, LayerType::BackgroundLayer, GameObjectType::BackgroundObject);

	Discoball* discoball = new Discoball(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "Assets/2D/discoball.png", sf::Vector2f(54, 54), sf::Vector2f(27, 27));
	(*CurrentGame).addToScene(discoball, LayerType::DiscoballLayer, GameObjectType::DiscoballObject);

	GameObject* goal_blue = new GameObject(sf::Vector2f(8, 540), sf::Vector2f(0, 0), "Assets/2D/goal_blue.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_blue, LayerType::GoalLayer, GameObjectType::GoalBlueObject);

	GameObject* goal_red = new GameObject(sf::Vector2f(1912, 540), sf::Vector2f(0, 0), "Assets/2D/goal_red.png", sf::Vector2f(16, 200), sf::Vector2f(8, 100));
	(*CurrentGame).addToScene(goal_red, LayerType::GoalLayer, GameObjectType::GoalRedObject);
}

void InGameState::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);

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