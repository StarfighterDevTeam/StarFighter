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
			Individual hero;
			for (int genId = 0; genId < NB_GENERATIONS; genId++)
			{
				static Generation current_gen = Generation(0);

				//Loop through individuals
				for (int individualId = 0; individualId < POPULATION_SIZE; individualId++)
				{
					if (HUMAN_PLAYER_ONLY == false)
						printf("Gen: %d, Individual: %d", genId, individualId);

					newgame.reset();
					bool bGameOver = false;
					bool bEvolutionOver = false;

					//Game loop
					while (!bGameOver || bEvolutionOver || HUMAN_PLAYER_ONLY)
					{
						Action action = Action::STRAIGHT;

						const Individual& current_individual = bEvolutionOver ? hero : current_gen.m_population[individualId];

						if (HUMAN_PLAYER_ONLY == false)
							action = newgame.getAction(newgame.getState(), current_individual);

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
								bEvolutionOver = false;
								genId = 0;
								individualId = 0;
								printf("Restart using last generation");
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

						const int score = newgame.update(dt, action, bEvolutionOver);

						if (HUMAN_PLAYER_ONLY == false)
						{
							if (score >= 0)
							{
								bGameOver = true;
								if (!bEvolutionOver)
									current_gen.m_population[individualId].setFitness(score);
								printf(" Score: %d\n", score);
							}
						}

						if (bEvolutionOver)
						{
							newgame.draw();
							renderWindow.display();
						}

						//end of evolution? loop play with Hero DNA
						if (!bEvolutionOver && individualId == POPULATION_SIZE - 1 && genId == NB_GENERATIONS - 1)
						{
							printf("\n");
							system("pause");
							bEvolutionOver = true;
							current_gen.OrderPopulation();
							if (hero.m_fitness < current_gen.m_population[POPULATION_SIZE - 1].getFitness())//keep hero
								hero = current_gen.m_population[POPULATION_SIZE - 1];

							printf("--- Hero playing (top score: %d)---\n", hero.getFitness());
						}
					}
				}

				//Select best individuals
				current_gen.OrderPopulation();

				if (hero.m_fitness < current_gen.m_population[POPULATION_SIZE - 1].getFitness())//keep hero
					hero = current_gen.m_population[POPULATION_SIZE - 1];

				const int averageScore = current_gen.getAverageFitness();
				printf("--- Generation results: top score: %d, average score: %d ---\n\n", current_gen.m_population[POPULATION_SIZE - 1].getFitness(), averageScore);

				//Evoluate
				if (genId < NB_GENERATIONS - 1)
					Generation::evoluate(current_gen, hero, 10);
			}
		}
	}

	return 0;
}