#include "StarEngine.h"
#include "InGameState.h"
#include "Inputs.h"
#include "Ship.h"
#include "Game.h"

#define ICON_SHIP_PNG							"Icons/ShipModel_icon.png"

SEInGameState* CreateInGameState()
{
	return new InGameState;
}

SEInputs* CreateInputs()
{
	return new Inputs;
}

SEShip* CreateShip()
{
	return new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
}

SEGame* CreateGame()
{
	return new Game();
}

int main()
{
	StarEngine	engine;
	engine.Init("Proto", ICON_SHIP_PNG);
	engine.Run();
	engine.Shut();
	return 0;
}
