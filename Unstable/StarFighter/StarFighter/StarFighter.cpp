#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <cmath>
#include "Ship.h"
#include "Background.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	Ship myShip;
	Background myBackground;

	myShip.Init(0,0);
	myBackground.Init(0,0);


	//update
	sf::Time dt;
	sf::Clock deltaClock;

    while (window.isOpen())
    {

		bool moving = false;
		sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();

		dt = deltaClock.restart();
		myShip.Update(dt.asSeconds());


		//display
        window.clear();
		window.draw(myBackground);
		window.draw(myShip);
		
        window.display();
    }

    return 0;
}