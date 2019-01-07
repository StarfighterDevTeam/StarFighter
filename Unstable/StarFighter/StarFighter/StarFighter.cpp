#include "StarFighter.h"

#ifdef WIN32
	#include "resource.h"
#endif

Game* CurrentGame;

int main()
{
	LOGGER_START(Logger::DEBUG, "");

	//Load Configuration
	LOGGER_WRITE(Logger::DEBUG, "Loading Configurations");
	PREFS = new PrefsManager();

	//Init SFML Window
	LOGGER_WRITE(Logger::DEBUG, "Initializing SFML Window");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);

	//Refresh rate
	renderWindow.setFramerateLimit(PREFS->m_gameRefreshRateHz);

	//Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Title
	renderWindow.setTitle("Giant Robot People - Prototype");

	//Game initialization
	CurrentGame = new Game();

	//Random seed
	srand(time(NULL));

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	//Initializing player
	LOGGER_WRITE(Logger::DEBUG, "Initializing player");
	Player player;
	player.Init(&renderWindow);

	//Loading InGame state
	LOGGER_WRITE(Logger::DEBUG, "Starting game");
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
	LOGGER_WRITE(Logger::DEBUG, "Initialization complete. Starting main loop...");

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
			else if (event.type == sf::Event::GainedFocus)
			{
				(*CurrentGame).m_window_has_focus = true;
				LOGGER_WRITE(Logger::DEBUG, "Window focus gained");
			}
			else if (event.type == sf::Event::LostFocus)
			{
				(*CurrentGame).m_window_has_focus = false;
				LOGGER_WRITE(Logger::DEBUG, "Window focus lost");
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
					renderWindow.create(VideoMode(1600, 900), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1600, 900);
					break;
				}
				case RESOLUTION_1920x1080_FullScreen:
				{
					fullscreen = true;
					renderWindow.create(VideoMode(1920, 1080), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1920, 1080);
					break;
				}
				case RESOLUTION_1280x720:
				{
					fullscreen = false;
					renderWindow.create(VideoMode(1280, 720), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1280, 720);
					break;
				}
				case RESOLUTION_1920x1080:
				{
					fullscreen = false;
					renderWindow.create(VideoMode(1920, 1080), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1920, 1080);
					break;
				}
			}

			//setting parameters again, because they are lost on calling renderWindow.create
			renderWindow.setKeyRepeatEnabled(false);
			renderWindow.setFramerateLimit(PREFS->m_gameRefreshRateHz);
			sf::Image icon = sf::Image();
			icon.loadFromFile(makePath(ICON_SHIP_PNG));
			renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
			renderWindow.setTitle("StarFighter Engine");
		}

		if ((*CurrentGame).m_playerShip)
		{
			//Muting
			(*CurrentGame).m_playerShip->GetInputState(InputGuy::isMuting(), Action_Muting);
			if ((*CurrentGame).m_playerShip->UpdateAction(Action_Muting, Input_Tap, true))
			{
				(*CurrentGame).SetMusicVolume(!(*CurrentGame).m_playerShip->m_actions_states[Action_Muting]);
				(*CurrentGame).SetSFXVolume(!(*CurrentGame).m_playerShip->m_actions_states[Action_Muting]);
			}
		}

		dt = deltaClock.restart();

		if (dt.asSeconds() < 0.3f)
		{
			if (!(*CurrentGame).m_pause)
			{
				//Update
				gameManager.GetCurrentState()->Update(dt);

				//Draw
				gameManager.GetCurrentState()->Draw();
				//sfgui.Display(renderWindow);

				//Diplay
				renderWindow.display();
			}
		}
		else
		{
			LOGGER_WRITE(Logger::DEBUG, "FRAME RATE TOO LOW - GAME WAS PAUSED FOR A MOMENT");
		}
	}

	return 0;
}