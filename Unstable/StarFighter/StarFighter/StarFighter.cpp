#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );

	CurrentGame = new Game();
	(*CurrentGame).init(window);

	//adding background
	Independant* bg = new Independant(sf::Vector2f(0,1200),sf::Vector2f(0,+10),BACKGROUND_FILENAME,Vector2f(800,1800),Vector2f(0,0));
	(*CurrentGame).addToScene(bg);

	Independant* badguy = new Independant(sf::Vector2f(400,100),sf::Vector2f(0,+20),ENEMYX_FILENAME,Vector2f(32,32),Vector2f(0,0));
	(*CurrentGame).addToScene(badguy);

	Ship myShip;
	myShip.Init(400,500);

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