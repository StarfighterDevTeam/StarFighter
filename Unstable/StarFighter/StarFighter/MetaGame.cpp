#include "MetaGame.h"

extern Game* CurrentGame;

MetaGame::MetaGame(sf::RenderWindow* window, string m_currentScene_name)
{
	this->mainWindow = window;
	(*CurrentGame).init(window);

	this->currentScene = new Scene(m_currentScene_name);

	this->playerShip = new Ship(Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO,SCENE_SIZE_Y*STARTSCENE_Y_RATIO), *FileLoader::LoadShipConfig("default"));
	(*CurrentGame).SetPlayerShip(this->playerShip);

	LOGGER_WRITE(Logger::Priority::DEBUG,"Playership loaded\n");

	//bg
	this->currentScene->bg->setPosition(0,this->currentScene->bg->getPosition().y + SCENE_SIZE_Y); // this is an exception for the 1st level of the game only
	this->currentScene->hub->setPosition(0,this->currentScene->hub->getPosition().y + SCENE_SIZE_Y);
	(*CurrentGame).addToScene(this->currentScene->bg,LayerType::BackgroundLayer,IndependantType::Background);
	(*CurrentGame).addToScene(this->currentScene->hub,LayerType::BackgroundLayer,IndependantType::Background);

	//ship
	(*CurrentGame).addToScene((*CurrentGame).playerShip,LayerType::PlayerShipLayer, IndependantType::PlayerShip);
	playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
	playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);

	hud = new PlayerHud();

	hud->Init(this->playerShip->ship_config.getShipConfigArmor(), this->playerShip->ship_config.getShipConfigShield());
}

void MetaGame::Update(Time deltaTime)
{
	this->currentScene->Update(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());
	(*CurrentGame).updateScene(deltaTime);
	this->mainWindow->clear();
	(*CurrentGame).drawScene();
	hud->update(this->mainWindow, (*CurrentGame).playerShip->ship_config.getShipConfigArmor(),(*CurrentGame).playerShip->ship_config.getShipConfigShield(),(*CurrentGame).playerShip->getMoney(),
		(*CurrentGame).playerShip->getGrazeCount(), (*CurrentGame).getHazard(), this->currentScene->getSceneHazardBreakValue(), deltaTime);
	this->mainWindow->display();
}

void MetaGame::LoadNextScene(string m_nextScene_name)
{
	//free(this->nextScene);
	this->nextScene = new Scene(m_nextScene_name);
}