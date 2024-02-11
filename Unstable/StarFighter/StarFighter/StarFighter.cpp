#include "StarFighter.h"

#define HUMAN_PLAYER_ONLY	false

int main()
{
	//SFML Window
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Snake AI");
	renderWindow.setKeyRepeatEnabled(false);
	renderWindow.setFramerateLimit(60);
	//renderWindow.setTitle("StarFighter Beta");

	//Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Game
	sf::Time dt;
	sf::Clock deltaClock;
	Game newgame = Game();
	newgame.init(&renderWindow);

	//Random seed
	srand(time(NULL));

	//Main loop
	while (renderWindow.isOpen())
	{
		//Generations
		{
			//Loop through generations
			for (int genId = 0; genId < 2; genId++)
			{
				Generation current_gen = Generation(genId);

				//Loop through individuals
				for (int individualId = 0; individualId < POPULATION_SIZE; individualId++)
				{
					if (HUMAN_PLAYER_ONLY == false)
						printf("Gen: %d, Individual: %d", genId, individualId);

					newgame.reset();
					bool bGameOver = false;

					//Game loop
					while (!bGameOver || HUMAN_PLAYER_ONLY)
					{
						Action action = Action::STRAIGHT;

						if (HUMAN_PLAYER_ONLY == false)
							action = newgame.getAction(newgame.getState(), &current_gen.m_population[individualId]);

						//human inputs
						sf::Event event;
						while (renderWindow.pollEvent(event))
						{
							if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
							{
								renderWindow.close();
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
							{
								newgame.reset();
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
							{
								//newgame.setPause(!newgame.isPaused());
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
							{
								if (newgame.getPlayerDirection() == Direction::UP)
									action = Action::TURN_LEFT;
								else if (newgame.getPlayerDirection() == Direction::DOWN)
									action = Action::TURN_RIGHT;
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
							{
								if (newgame.getPlayerDirection() == Direction::UP)
									action = Action::TURN_RIGHT;
								else if (newgame.getPlayerDirection() == Direction::DOWN)
									action = Action::TURN_LEFT;
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
							{
								if (newgame.getPlayerDirection() == Direction::LEFT)
									action = Action::TURN_RIGHT;
								else if (newgame.getPlayerDirection() == Direction::RIGHT)
									action = Action::TURN_LEFT;
							}

							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
							{
								if (newgame.getPlayerDirection() == Direction::LEFT)
									action = Action::TURN_LEFT;
								else if (newgame.getPlayerDirection() == Direction::RIGHT)
									action = Action::TURN_RIGHT;
							}
						}

						dt = deltaClock.restart();

						const int score = newgame.update(dt, action);

						if (HUMAN_PLAYER_ONLY == false)
						{
							if (score >= 0)
							{
								bGameOver = true;
								current_gen.m_population[individualId].setFitness(score);
								printf(" Score: %d\n", score);
							}
						}

						newgame.draw();
						renderWindow.display();
					}
				}
			}
		}
	}

	//while (hero.m_fitness < MAX_FITNESS || current_gen.m_gen == 0)
	//{
	//	printf("\nGeneration %d. ", current_gen.m_gen + 1);
	//	current_gen.CreateNewGeneration(hero);
	//	current_gen.ComputeFitness(secret);
	//	current_gen.OrderPopulation();
	//	hero = current_gen.m_population[POPULATION_SIZE - 1];
	//
	//	printf("HERO dna: ");
	//	hero.DisplayDNA();
	//	printf(", Fitness: %d\n", hero.m_fitness);
	//	
	//}
	//
	//printf("\nSUCESS.\n");
	//hero.DisplayEvolutionRecord(secret.m_dna);
	//printf("\n");
	//system("pause");

	return 0;
}