#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );

	CurrentGame = new Game();
	(*CurrentGame).init(window);

	//adding background
	Independant* bg = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,+10),BACKGROUND_FILENAME,Vector2f(800,600),Vector2f(0,0));
	(*CurrentGame).addToScene(bg);

	Ship myShip;
	myShip.Init(0,0);

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	while (window->isOpen())
	{

		bool moving = false;
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window->close();

		dt = deltaClock.restart();

		(*CurrentGame).updateScene(dt);
		myShip.Update(dt);

		//display
		window->clear();

		(*CurrentGame).drawScene();
		window->draw(myShip);

		window->display();
	}

	return 0;
}