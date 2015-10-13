#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Ship.h"

class Independant;

class InGameState : public GameState
{
public:
	Independant* playerShip;

	void Initialize(Player player);
	void Update(sf::Time deltaTime);
	void Draw();
	void Release();

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED