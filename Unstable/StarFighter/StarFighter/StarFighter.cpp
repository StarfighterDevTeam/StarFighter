#include "StarFighter.h"

int main()
{
	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );

	Ship myShip;
	Background myBackground;
	
	myBackground.Init(0,0);
	for (int i=0; i<LASER_MAX_AMMO_PER_STAGE; i++)
	{
		myShip.laser[i].Init(0,0);
	}
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

		myShip.Update(dt);

		for (int i=0; i<LASER_MAX_AMMO_PER_STAGE; i++)
		{
			myShip.laser[i].Update(dt.asSeconds());
		}

		//display
        window->clear();
		window->draw(myBackground);
		window->draw(myShip);

		for (int i=0; i<LASER_MAX_AMMO_PER_STAGE; i++)
		{
			window->draw(myShip.laser[i]);
		}
        window->display();
    }

    return 0;
}