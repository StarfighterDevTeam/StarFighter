#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <cmath>
#include "Globals.h"
#include "Ship.h"
#include "Background.h"
#include "Weapon.h"

int main()
{
	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );
	window.setFramerateLimit(60);
	// window( new sf::RenderWindow( sf::VideoMode(800,600), "Test" ) );

	Ship myShip;
	Background myBackground;


	
	myBackground.Init(0,0);
	myShip.laser.Init(0,0);
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
		myShip.laser.Update(dt.asSeconds());

		//display
        window->clear();
		window->draw(myBackground);
		window->draw(myShip);
		window->draw(myShip.laser);
        window->display();
    }

    return 0;
}