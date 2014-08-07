#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "Scene.h"
#include "Hud.h"

class MetaGame
{
public:
	Ship* playerShip;
	Scene* currentScene;
	Scene* nextScene;
	PlayerHud* hud;

	MetaGame::MetaGame(sf::RenderWindow* window, string m_currentScene_name);
	void MetaGame::Update(Time deltaTime);
	void MetaGame::LoadNextScene(string m_nextScene_name);

	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED