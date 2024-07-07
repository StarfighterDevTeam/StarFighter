#include "StarFighter.h"

int main()
{
	//SFML Window
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Snake AI");
	renderWindow.setKeyRepeatEnabled(false);
	renderWindow.setFramerateLimit(60);
	//renderWindow.setTitle("StarFighter Beta");

	//Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Random seed
	srand(time(NULL));

	//Game
	sf::Time dt;
	sf::Clock deltaClock;
	Game currentGame = Game();
	currentGame.init(&renderWindow);

	//Main loop
	while (renderWindow.isOpen())
	{
		//Generations
		{
			//update game
			dt = deltaClock.restart();
			currentGame.update(dt);

			//display
			{
				currentGame.draw();
				renderWindow.display();
			}
		}
	}

	return 0;
}