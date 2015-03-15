#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	LOGGER_START(Logger::Priority::DEBUG, "");

	//Load Configuration
	LOGGER_WRITE(Logger::Priority::DEBUG, "Loading Configurations");
	PREFS = new PrefsManager();

	//Init SFML Window
	LOGGER_WRITE(Logger::Priority::DEBUG, "Initializing GUI");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);
	//Refresh rate
	renderWindow.setVerticalSyncEnabled(true);
	renderWindow.setFramerateLimit(PREFS->m_gameRefreshRateHz);

	//Init SFGUI Window
	sfg::SFGUI sfgui;
	auto mainWindow = sfg::Window::Create();
	mainWindow->SetTitle("Starfighter");

	//Test
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.f);
	auto m_button = sfg::Button::Create("Chips au jambon");
	box->Pack(m_button);
	mainWindow->Add(box);

	//Music
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
	Player player;
	player.Init(&renderWindow);
	player.m_currentSceneFile = "Vanguard_Hub0";

	//Loading InGame state
	LOGGER_WRITE(Logger::Priority::DEBUG, "Starting game");
	GameManager gameManager;
	InGameState inGameState;
	gameManager.PushState(inGameState, player);

	while (renderWindow.isOpen())
	{
		//Sampling
		//if (deltaClock.getElapsedTime().asMilliseconds() < 18)
		//	continue;

		sf::Event event;
		while (renderWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				renderWindow.close();
			}
			mainWindow->HandleEvent(event);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			renderWindow.close();
		}

		dt = deltaClock.restart();

		//Update
		mainWindow->Update(dt.asSeconds());
		gameManager.GetCurrentState()->Update(dt);

		//Draw
		gameManager.GetCurrentState()->Draw();
		//sfgui.Display(renderWindow);

		//Diplay
		renderWindow.display();
	}

	return 0;
}