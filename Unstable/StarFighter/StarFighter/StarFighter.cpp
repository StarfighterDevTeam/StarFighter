#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	LOGGER_START(Logger::Priority::DEBUG, "");

	//Load Configuration
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Configurations");
	PREFS = new PrefsManager();

	//Init SFML Window
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initializing SFML Window");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);
	//Refresh rate
	//renderWindow.setVerticalSyncEnabled(true);
	renderWindow.setFramerateLimit(PREFS->m_gameRefreshRateHz);

	//Music
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Musics");
	sf::Music SpaceCowboys;
	if (!SpaceCowboys.openFromFile("Assets/Music/SpaceCowboys.ogg"))
		//if (!SpaceCowboys.openFromFile("Assets/Music/CrimeaDigital.ogg"))
		//if (!SpaceCowboys.openFromFile("Assets/Music/Rebecca.ogg"))
		//if (!SpaceCowboys.openFromFile("Assets/Music/OrientalCrystal.ogg"))
		return -1; // erreur
	SpaceCowboys.play();
	SpaceCowboys.setLoop(true);

	CurrentGame = new Game();
	srand(time(NULL));

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	//Initializing player
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initializing player");
	Player player;
	player.Init(&renderWindow);
	player.m_currentSceneFile = "Vanguard_Hub0";

	//Loading InGame state
	LOGGER_WRITE(Logger::Priority::DEBUG, "Starting game");
	GameManager gameManager;
	InGameState inGameState;
	gameManager.PushState(inGameState, player);

	//Handling various window resolutions
	enum WindowResolutions
	{
		RESOLUTION_1600x900,
		RESOLUTION_1920x1080_FullScreen,
		RESOLUTION_1280x720,
		RESOLUTION_1920x1080,
		NBVAL_RESOLUTIONS,
	};
	bool fullscreen = false;
	WindowResolutions resolution = RESOLUTION_1600x900;
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initialization complete. Starting main loop...");

	//Main loop
	while (renderWindow.isOpen())
	{
		sf::Event event;
		while (renderWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				renderWindow.close();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			renderWindow.close();
		}

		//Resolution switch
		if (InputGuy::isChangingResolution())
		{
			resolution = (WindowResolutions)(((int)resolution +1) % (NBVAL_RESOLUTIONS - 1));
			switch (resolution)
			{
				case RESOLUTION_1600x900:
				{
					fullscreen = false;
					renderWindow.create(VideoMode(1600, 900), "Starfighter", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).screen_size = sf::Vector2i(1600, 900);
					break;
				}
				case RESOLUTION_1920x1080_FullScreen:
				{
					fullscreen = true;
					renderWindow.create(VideoMode(1920, 1080), "Starfighter", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).screen_size = sf::Vector2i(1920, 1080);
					break;
				}
				case RESOLUTION_1280x720:
				{
					fullscreen = false;
					renderWindow.create(VideoMode(1280, 720), "Starfighter", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).screen_size = sf::Vector2i(1280, 720);
					break;
				}
				case RESOLUTION_1920x1080:
				{
					fullscreen = false;
					renderWindow.create(VideoMode(1920, 1080), "Starfighter", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).screen_size = sf::Vector2i(1920, 1080);
					break;
				}
			}
		}

		dt = deltaClock.restart();

		//Update
		gameManager.GetCurrentState()->Update(dt);

		//Draw
		gameManager.GetCurrentState()->Draw();
		//sfgui.Display(renderWindow);

		//Diplay
		renderWindow.display();
	}

	return 0;
}