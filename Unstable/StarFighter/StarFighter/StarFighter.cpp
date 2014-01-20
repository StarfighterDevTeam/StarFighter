#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );
	glEnable(GL_TEXTURE_2D);
	window->setKeyRepeatEnabled(false);

	CurrentGame = new Game();
	(*CurrentGame).init(window);

	//adding background
	Independant* bg = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,+10),BACKGROUND_FILENAME,Vector2f(800,1800),Vector2f(0,1200));
	bg->setVisible(true);
	(*CurrentGame).addToScene(bg,LayerType::BackgroundLayer,IndependantType::Background);

	srand (time(NULL));

	ShipConfig* shipA;
	shipA = new ShipConfig();
	shipA->Init(sf::Vector2f(1000.0f, 1000.0f), SHIP_DECCELERATION_COEF, SHIP_FILENAME, sf::Vector2f(SHIP_WIDTH,SHIP_HEIGHT), SHIP_NB_FRAMES);
	ShipConfig* shipB;
	shipB = new ShipConfig();
	shipB->Init(sf::Vector2f(200.0f, 200.0f), SHIP_DECCELERATION_COEF, SHIP_FILENAME, sf::Vector2f(SHIP_WIDTH,SHIP_HEIGHT), SHIP_NB_FRAMES);

	Ship myShip(Vector2f(400,500),*shipB);
	(*CurrentGame).addToScene(&myShip,LayerType::PlayerShipLayer,IndependantType::PlayerShip);

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
			myShip.setShipConfig(*shipA);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
			myShip.setShipConfig(*shipB);

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window->close();

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		{
			Enemy* badguy = new Enemy(sf::Vector2f(rand() % WINDOW_RESOLUTION_X,-180),sf::Vector2f(0,+70),ENEMYX_FILENAME,Vector2f(32,32));
			badguy->collider_type = IndependantType::EnemyObject;
			badguy->setVisible(true);
			(*CurrentGame).addToScene(badguy,LayerType::EnemyObjectLayer,IndependantType::EnemyObject);
		}

		dt = deltaClock.restart();

		(*CurrentGame).updateScene(dt);
		//myShip.Update(dt);

		//display
		window->clear();

		(*CurrentGame).drawScene();
		//window->draw(myShip);

		window->display();
	}

	return 0;
}