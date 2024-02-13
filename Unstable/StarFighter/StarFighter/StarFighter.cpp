#include "StarFighter.h"

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
			static bool bWritingDNA = WRITE_DNA;
			Individual hero;
			if (hero.loadFromFile())
				printf("--- Hero loaded (score: %d) ---\n\n", hero.getFitness());

			int eraId = 0;
			for (int genId = 0; genId < NB_GENERATIONS; genId++)
			{
				static Generation current_gen = Generation(0);

				//Loop through individuals
				for (int individualId = 0; individualId < POPULATION_SIZE; individualId++)
				{
					if (HUMAN_PLAYER_ONLY == false)
						printf("Gen: %d, Individual: %d", genId + eraId * NB_GENERATIONS, individualId);

					newgame.reset();
					bool bGameOver = false;
					bool bEvolutionOver = false;

					//Game loop
					while (!bGameOver || bEvolutionOver || HUMAN_PLAYER_ONLY)
					{
						Action action = Action::STRAIGHT;

						Individual& current_individual = bEvolutionOver ? hero : current_gen.m_population[individualId];
						const int stateId = newgame.computeStateId();//compute game state into a decimal input value
						if (HUMAN_PLAYER_ONLY == false && bWritingDNA == false && USE_BEHARIOR_AI == false)
							action = newgame.getAction(stateId, current_individual);
						else if (HUMAN_PLAYER_ONLY == false && USE_BEHARIOR_AI == true)
							action = newgame.getAction_BehaviorAI(newgame.getState(), current_individual);

						//human inputs
						sf::Event event;
						bool bShouldQuit = false;
						while (renderWindow.pollEvent(event))
						{
							if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
							{
								bShouldQuit = true;
								renderWindow.close();
							}

							if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R)
							{
								newgame.reset();
								bEvolutionOver = false;
								genId = 0;
								individualId = 0;
								printf("Restart using last generation");
							}

							if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W)
							{
								bWritingDNA = !bWritingDNA;
								if (bWritingDNA)
									printf("\n--- Writing DNA ---");
								else
									printf("\n--- STOP Writing DNA ---");
							}

							if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::E)
							{
								newgame.reset();
								bEvolutionOver = !bEvolutionOver;
								if (bEvolutionOver)
									printf("\n--- Hero playing (top score: %d)---\n", hero.getFitness());
							}

							if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P)
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

						//write DNA
						if (bWritingDNA)
						{
							if (bShouldQuit)
							{
								Individual previous_hero;
								if (!previous_hero.loadFromFile() || previous_hero.getFitness() < hero.getFitness())
									hero.saveInFile();
							}

							hero.m_dna[stateId] = (int)action;
						}

						//update game
						dt = deltaClock.restart();
						int score;
						Death death;
						newgame.update(dt, action, bEvolutionOver || bWritingDNA, bWritingDNA, score, death, bGameOver);

						current_individual.setFitness(score);

						if (bGameOver)
							newgame.reset();

						if (HUMAN_PLAYER_ONLY == false)
						{
							//game over => compute score and display reason for death
							if (bGameOver)
							{
								const bool bIsHero = genId > 0 && individualId == POPULATION_SIZE - 1;
								if (&current_individual == &hero && !bWritingDNA)//don't re-rate the Hero
									current_individual.setFitness(score);
								char death_str[128];
								switch (death)
								{
									case Death::WALL:
									{
										std::strcpy(death_str, "Wall");
										current_gen.m_population[individualId].setBadGene(stateId);
										break;
									}
									case Death::OWN:
									{
										std::strcpy(death_str, "Own body");
										current_gen.m_population[individualId].setBadGene(stateId);
										break;
									}
									case Death::TIMEOUT:
									{
										std::strcpy(death_str, "Time out");
										break;
									}
								}
								printf(" Score: %d, Death: %s\n", score, death_str);
							}
						}

						//display
						if (bEvolutionOver || bWritingDNA)
						{
							newgame.draw();
							renderWindow.display();
						}

						//end of evolution? loop play with Hero DNA
						if (!bEvolutionOver && !bWritingDNA && individualId == POPULATION_SIZE - 1 && genId == NB_GENERATIONS - 1)
						{
							printf("\n");
							//system("pause");
							bEvolutionOver = true;
							eraId++;
							current_gen.OrderPopulation();
							if (hero.m_fitness < current_gen.m_population[POPULATION_SIZE - 1].getFitness())//keep hero
							{
								hero = current_gen.m_population[POPULATION_SIZE - 1];

								Individual previous_hero;
								if (!previous_hero.loadFromFile() || previous_hero.getFitness() < hero.getFitness())
									hero.saveInFile();
							}

							printf("--- Hero playing (top score: %d)---\n", hero.getFitness());
						}
					}
				}

				//Select best individuals
				current_gen.OrderPopulation();

				if (hero.m_fitness < current_gen.m_population[POPULATION_SIZE - 1].getFitness())//keep hero
				{
					hero = current_gen.m_population[POPULATION_SIZE - 1];

					Individual previous_hero;
					if (!previous_hero.loadFromFile() || previous_hero.getFitness() < hero.getFitness())
						hero.saveInFile();
				}

				const int averageScore = current_gen.getAverageFitness();
				printf("--- Generation results: top score: %d, average score: %d ---\n\n", current_gen.m_population[POPULATION_SIZE - 1].getFitness(), averageScore);

				//Evoluate
				if (genId < NB_GENERATIONS - 1)
					current_gen.evoluate(hero);
			}
		}
	}

	return 0;
}