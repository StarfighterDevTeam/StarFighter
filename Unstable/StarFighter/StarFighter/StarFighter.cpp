#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );
	glEnable(GL_TEXTURE_2D);
	window->setKeyRepeatEnabled(false);

	LOGGER_START(Logger::Priority::DEBUG, "");
	LOGGER_WRITE(Logger::Priority::DEBUG, "Starting game");

	sf::Music SpaceCowboys;
	if (!SpaceCowboys.openFromFile("Assets/Music/SpaceCowboys.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/CrimeaDigital.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Rebecca.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/OrientalCrystal.ogg"))
		return -1; // erreur
	SpaceCowboys.play();
	SpaceCowboys.setLoop(true);

	CurrentGame = new Game();
	srand (time(NULL));

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	//to erase later...
	sf::Clock deltaClockKeyPressed;
	bool keyrepeat = false;
	///...until here (avoiding key repetition)

	//Loading Scene
	Scene level1("Assets/Scripts/Scenes/scene1.csv");
	level1.StartGame(window);

	while (window->isOpen())
	{
		//Sampling
		if(deltaClock.getElapsedTime().asMilliseconds() < LOOP_SAMPLING_MS)
			continue;

		//to erase later...
		if (deltaClockKeyPressed.getElapsedTime() > sf::seconds(1))
		{
			keyrepeat = false;
		}
		// ... until here (avoiding key repeatition)

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window->close();

		dt = deltaClock.restart();
		level1.Update(dt);

	}

	return 0;
}