#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "Scene.h"
#include "Hud.h"
#include "GameState.h"

class InGameState : public GameState
{
public:
	Ship* playerShip;
	Scene* currentScene;
	Scene* nextScene;
	PlayerHud* hud;

	InGameStateMachine IG_State;

	void Initialize(Player player);
	void Update(Time deltaTime);
	void Draw();
	void Release();

	void InGameStateMachineCheck();
	sf::Clock clockHubExit;
	
private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED