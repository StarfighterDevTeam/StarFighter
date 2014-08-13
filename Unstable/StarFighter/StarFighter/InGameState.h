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
	void InGameState::Scrolling_to_LastScreen();
	void InGameState::LastScreen_to_Transition();
	void InGameState::Transition1_2();
	void InGameState::Transition2_2();
	sf::Vector2f InGameState::StartingShipPosition(sf::Vector2i direction);

	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED