#include "StarFighter.h"

#ifdef WIN32
	#include "resource.h"
#endif

Game* CurrentGame;

int carre[9];
int sum = -1;
int try_count = 0;
int max_number = 30;

void InitCarre(int a, int b, int c)
{
	carre[0] = a;
	carre[1] = b;
	carre[2] = c;

	sum = carre[0] * carre[0] + carre[1] * carre[1] + carre[2] * carre[2];
	printf("Init: a=%d, b=%d, c=%d | sum=%d\n", a, b, c, sum);
}

int ComputeCarre6()
{
	for (int i = 1; i < max_number; i++)
	{
		if (i == carre[0] || i == carre[1] || i == carre[2] || i == carre[4] || i == carre[7] || i == carre[8])
		{
			continue;
		}

		try_count++;
		int sum2 = carre[2] * carre[2] + carre[4] * carre[4] + i*i;
		if (sum2 == sum)
		{
			carre[6] = i;
			printf("\n\n## MATCH ##\n\n");
			return i;
		}
		else
		{
			try_count++;
			printf("   try %d: i=%d; j=%d; k=%d; l=%d| sum=%d \n", try_count, carre[4], carre[7], carre[8], i, sum2);
		}
	}

	return -1;
}

int ComputeCarre8()
{
	for (int i = 1; i < max_number; i++)
	{
		if (i == carre[0] || i == carre[1] || i == carre[2] || i == carre[4] || i == carre[7])
		{
			continue;
		}

		try_count++;
		int sum2 = carre[0] * carre[0] + carre[4] * carre[4] + i*i;
		if (sum2 == sum)
		{
			carre[8] = i;
			//printf("\n\n## MATCH ##\n\n");
			ComputeCarre6();
			return i;
		}
		else
		{
			try_count++;
			printf("   try %d: i=%d; j=%d; k=%d| sum=%d \n", try_count, carre[4], carre[7], i, sum2);
		}
	}

	return -1;
}

sf::Vector2i ComputeCarre4And7()
{
	for (int i = 1; i < max_number; i++)
	{
		if (i == carre[0] || i == carre[1] || i == carre[2])
		{
			continue;
		}


		for (int j = 1; j < max_number; j++)
		{
			if (j == carre[0] || j == carre[1] || j == carre[2] || j == i)
			{
				continue;
			}

			int sum2 = carre[1] * carre[1] + i*i + j*j;
			
			if (sum2 == sum)
			{
				carre[4] = i;
				carre[7] = j;
				//printf("\n\n## MATCH ##\n\n");
				ComputeCarre8();
				return sf::Vector2i(i, j);
			}
			else
			{
				try_count++;
				printf("   try %d: i=%d; j=%d | sum=%d \n", try_count, i, j, sum2);
			}
		}
	}

	return sf::Vector2i(-1, -1);
}



int main()
{
	//LOGGER_START(Logger::DEBUG, "");
	//
	////Load Configuration
	//LOGGER_WRITE(Logger::DEBUG, "Loading Configurations");
	//PREFS = new PrefsManager();
	//
	////Init SFML Window
	//LOGGER_WRITE(Logger::DEBUG, "Initializing SFML Window");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);
	//
	////Refresh rate
	//renderWindow.setFramerateLimit(60);
	//
	////Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Title
	renderWindow.setTitle("StarFighter Engine");

	////Game initialization
	//CurrentGame = new Game();
	//
	////Random seed
	//srand(time(NULL));
	//
	////update
	//sf::Time dt;
	//sf::Clock deltaClock;
	//
	////Initializing player
	//LOGGER_WRITE(Logger::DEBUG, "Initializing player");
	//Player player;
	//player.Init(&renderWindow);
	//
	////Loading InGame state
	//LOGGER_WRITE(Logger::DEBUG, "Starting game");
	//GameManager gameManager;
	//InGameState inGameState;
	//gameManager.PushState(inGameState, player);
	//
	////Handling various window resolutions
	//enum WindowResolutions
	//{
	//	RESOLUTION_1600x900,
	//	RESOLUTION_1920x1080_FullScreen,
	//	RESOLUTION_1280x720,
	//	RESOLUTION_1920x1080,
	//	NBVAL_RESOLUTIONS,
	//};
	//bool fullscreen = false;
	//WindowResolutions resolution = RESOLUTION_1600x900;
	//LOGGER_WRITE(Logger::DEBUG, "Initialization complete. Starting main loop...");

	

	//Main loop
	while (renderWindow.isOpen())
	{
		int a = 1;
		int b = 2;
		int c = 3;

		for (int a = 1; a < max_number; a++)
		{
			for (int b = 1; b < max_number; b++)
			{
				if (b == a)
				{
					continue;
				}

				for (int c = 1; c < max_number; c++)
				{
					if (c == a || c == b)
					{
						continue;
					}

					InitCarre(a, b, c);
					ComputeCarre4And7();
				}
			}
		}

		while (1)
		{

		}
		
		
	}

	return 0;
}

