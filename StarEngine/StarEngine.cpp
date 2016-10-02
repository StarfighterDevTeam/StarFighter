#include "StarEngine.h"

SEGame* CurrentGame;

//Initialize TextureLoader Singleton
TextureLoader *TextureLoader::_singleton = NULL;

//Initialize Logger Singleton
Logger Logger::instance;
Logger::Logger() : active(false) {}

// --- To be implemented by the game ---
SEInGameState* CreateInGameState();
SEInputs* CreateInputs();
SEShip* CreateShip();
SEGame* CreateGame();
// -------------------------------------

bool StarEngine::Init(const char* title, const char* iconName, WindowResolutions defaultResolution)
{
	LOGGER_START(Logger::DEBUG, "");

	//Init SFML Window
	LOGGER_WRITE(Logger::DEBUG, "Initializing SFML Window");
	m_renderWindow = new sf::RenderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), title);
	m_title = title;
	m_renderWindow->setKeyRepeatEnabled(false);

	//Refresh rate
	m_frameRateLimit = 60;
	m_renderWindow->setFramerateLimit(m_frameRateLimit);

	//Icon
	m_iconName = iconName;
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(iconName));
	m_renderWindow->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	// Inputs initialization
	InputsManager = CreateInputs();

	//Game initialization
	CurrentGame = CreateGame();

	//Random seed
	srand(time(NULL));

	//Loading InGame state
	LOGGER_WRITE(Logger::DEBUG, "Starting game");
	m_inGameState = CreateInGameState();
	m_inGameState->Initialize(m_renderWindow, CreateShip());

	LOGGER_WRITE(Logger::DEBUG, "Initialization complete. Starting main loop...");

	return true;
}

void StarEngine::Run()
{
	bool fullscreen = false;
	WindowResolutions resolution = RESOLUTION_1600x900;

	//update
	sf::Time dt;
	sf::Clock deltaClock;
	
	//Main loop
	while (m_renderWindow->isOpen())
	{
		sf::Event event;
		while (m_renderWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				m_renderWindow->close();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			m_renderWindow->close();
		}

		//Resolution switch
		if (InputsManager->isChangingResolution())
		{
			resolution = (WindowResolutions)(((int)resolution +1) % (NBVAL_RESOLUTIONS - 1));
			switch (resolution)
			{
				case RESOLUTION_1600x900:
				{
					fullscreen = false;
					m_renderWindow->create(VideoMode(1600, 900), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1600, 900);
					break;
				}
				case RESOLUTION_1920x1080_FullScreen:
				{
					fullscreen = true;
					m_renderWindow->create(VideoMode(1920, 1080), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1920, 1080);
					break;
				}
				case RESOLUTION_1280x720:
				{
					fullscreen = false;
					m_renderWindow->create(VideoMode(1280, 720), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1280, 720);
					break;
				}
				case RESOLUTION_1920x1080:
				{
					fullscreen = false;
					m_renderWindow->create(VideoMode(1920, 1080), "StarFighter Engine", (fullscreen ? Style::Fullscreen : Style::Resize | Style::Close));
					(*CurrentGame).m_screen_size = sf::Vector2i(1920, 1080);
					break;
				}
			}

			//setting parameters again, because they are lost on calling m_renderWindow->create
			m_renderWindow->setKeyRepeatEnabled(false);
			m_renderWindow->setFramerateLimit(m_frameRateLimit);
			sf::Image icon = sf::Image();
			icon.loadFromFile(makePath(m_iconName));
			m_renderWindow->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
			m_renderWindow->setTitle(m_title);
		}

		if ((*CurrentGame).m_playerShip)
		{
			//Muting
			(*CurrentGame).m_playerShip->GetInputState(InputsManager->isMuting(), Action_Muting);
			if ((*CurrentGame).m_playerShip->UpdateAction(Action_Muting, Input_Tap, true))
			{
				(*CurrentGame).SetMusicVolume(!(*CurrentGame).m_playerShip->m_actions_states[Action_Muting]);
				(*CurrentGame).SetSFXVolume(!(*CurrentGame).m_playerShip->m_actions_states[Action_Muting]);
			}

			//Pausing
			(*CurrentGame).m_playerShip->GetInputState(InputsManager->isPausing(), Action_Pausing);
			if ((*CurrentGame).m_playerShip->UpdateAction(Action_Pausing, Input_Tap, true))
			{
				(*CurrentGame).m_pause = (*CurrentGame).m_playerShip->m_actions_states[Action_Pausing];

				if ((*CurrentGame).m_pause)
				{
					(*CurrentGame).m_curMusic.pause();
				}
				else
				{
					(*CurrentGame).m_curMusic.play();
				}
			}
		}

		dt = deltaClock.restart();

		if (!(*CurrentGame).m_pause)
		{
			//Update
			m_inGameState->Update(dt);

			//Draw
			m_inGameState->Draw();
			//sfgui.Display(renderWindow);

			//Diplay
			m_renderWindow->display();
		}
	}
}

void StarEngine::Shut()
{
	delete CurrentGame;
	m_inGameState->Release();
	delete m_inGameState;
	delete m_renderWindow;
}
