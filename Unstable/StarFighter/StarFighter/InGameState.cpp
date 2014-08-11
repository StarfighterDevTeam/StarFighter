#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	this->currentScene = new Scene(player.m_currentSceneFile, player.reverse_scene, true);//first_scene = true
	(*CurrentGame).SetScrollingDirection(this->currentScene->direction);

	this->playerShip = new Ship(Vector2f(SCENE_SIZE_X*STARTSCENE_X_RATIO, SCENE_SIZE_Y*STARTSCENE_Y_RATIO), *FileLoader::LoadShipConfig("default"));
	(*CurrentGame).SetPlayerShip(this->playerShip);

	LOGGER_WRITE(Logger::Priority::DEBUG, "Playership loaded\n");

	//bg
	(*CurrentGame).addToScene(this->currentScene->bg, LayerType::BackgroundLayer, IndependantType::Background);

	//ship
	(*CurrentGame).addToScene((*CurrentGame).playerShip, LayerType::PlayerShipLayer, IndependantType::PlayerShip);
	playerShip->ship_config.GenerateBots((*CurrentGame).playerShip);
	playerShip->ship_config.GenerateFakeShip((*CurrentGame).playerShip);

	hud = new PlayerHud();
	hud->Init(this->playerShip->ship_config.getShipConfigArmor(), this->playerShip->ship_config.getShipConfigShield());
}

void InGameState::Update(Time deltaTime)
{
	this->currentScene->Update(deltaTime);
	(*CurrentGame).GetBeastScoreBonus((*CurrentGame).playerShip->getShipBeastScore(), this->currentScene->getSceneBeastScore());
	(*CurrentGame).updateScene(deltaTime);
	this->mainWindow->clear();

	hud->Update(this->mainWindow, (*CurrentGame).playerShip->ship_config.getShipConfigArmor(), (*CurrentGame).playerShip->ship_config.getShipConfigShield(), (*CurrentGame).playerShip->getMoney(),
		(*CurrentGame).playerShip->getGrazeCount(), (*CurrentGame).getHazard(), this->currentScene->getSceneHazardBreakValue(), deltaTime);
}

void InGameState::Draw()
{
	(*CurrentGame).drawScene();
	hud->Draw(this->mainWindow);
}

void InGameState::Release()
{
	//TODO
}

void InGameState::LoadNextScene(string m_nextScene_name)
{
	//free(this->nextScene);
	this->nextScene = new Scene(m_nextScene_name);
}