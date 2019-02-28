#include "StarFighter.h"

#ifdef WIN32
	#include "resource.h"
#endif

Game* CurrentGame;

int main()
{
	//Init SFML Window
	printf("Initializing SFML Window");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);

	//Refresh rate
	renderWindow.setFramerateLimit(60);

	//Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Title
	renderWindow.setTitle("Pirates of the Seven Seas");

	//Random seed
	srand(time(NULL));

	//Game initialization
	CurrentGame = new Game(renderWindow);

	//Elapsed time
	sf::Time dt;
	sf::Clock deltaClock;

	//Loading InGame state
	printf("Starting game\n");
	Gameloop gameloop;

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
	printf("Initialization complete. Starting main loop...\n");

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
				printf("Window focus gained\n");
			}
			else if (event.type == sf::Event::LostFocus)
			{
				(*CurrentGame).m_window_has_focus = false;
				printf("Window focus lost\n");
			}
		}

		if ((*CurrentGame).m_window_has_focus == true)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				renderWindow.close();
			}

			//Resolution switch
			if (InputGuy::isChangingResolution())
			{
				resolution = (WindowResolutions)(((int)resolution + 1) % (NBVAL_RESOLUTIONS - 1));
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
				renderWindow.setFramerateLimit(60);
				sf::Image icon = sf::Image();
				icon.loadFromFile(makePath(ICON_SHIP_PNG));
				renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
				renderWindow.setTitle("StarFighter Engine");
			}
		}

		dt = deltaClock.restart();

		if (dt.asSeconds() < 0.3f)
		{
			//Update
			gameloop.Update(dt);

			//Draw
			gameloop.Draw();

			//Diplay
			renderWindow.display();
		}
		else
		{
			printf("FRAME RATE TOO LOW - GAME WAS PAUSED FOR A MOMENT\n");
		}
	}

	return 0;
}