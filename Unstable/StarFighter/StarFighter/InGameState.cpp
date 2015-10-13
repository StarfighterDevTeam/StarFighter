#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	//intégration placeholder
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "Assets/2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).SetPlayerShip(playerShip);
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, GameObjectType::PlayerShip);

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "Assets/2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, LayerType::BackgroundLayer, GameObjectType::BackgroundObject);

	Discoball* discoball = new Discoball(sf::Vector2f(1200, 540), sf::Vector2f(0, 0), "Assets/2D/discoball.png", sf::Vector2f(54, 54), sf::Vector2f(27, 27));
	(*CurrentGame).addToScene(discoball, LayerType::DiscoballLayer, GameObjectType::DiscoballObject);
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