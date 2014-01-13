#include <SFML/Graphics.hpp>
#include <tuple>
#include <stdio.h>
#include <cmath>
#include "Ship.h"
#include "Background.h"

int main()
{
	//We hardcode the windows size here, TBD later
	sf::Vector2i screenDimensions(800,600);
    sf::RenderWindow window(sf::VideoMode(screenDimensions.x, screenDimensions.y), "StarFighter");
	window.setFramerateLimit(60);

	Ship myShip;
	Background myBackground;

	
	myBackground.Init(0,0);
	myShip.Init(0,0);

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
		myShip.Update(dt);


		//display
        window.clear();
		window.draw(myBackground);
		window.draw(myShip);
		
        window.display();
    }

    return 0;
}