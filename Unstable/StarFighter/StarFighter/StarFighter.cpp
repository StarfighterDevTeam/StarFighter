#include "StarFighter.h"

int main()
{
	printf("MASTERMIND - Genetic Algorithm\n\n");

	//Random seed
	srand(time(NULL));

	//randomize secret
	Individual secret;
	printf("secret: ");
	secret.DisplayDNA();
	printf("\n");

	printf("\nGeneration 0. ");
	Generation current_gen = Generation();
	current_gen.ComputeFitness(secret);
	current_gen.OrderPopulation();
	Individual& hero = current_gen.m_population[99];
	printf("HERO dna: ");
	hero.DisplayDNA();
	printf(", Fitness: %d\n", hero.m_fitness);

	//Main loop
	while (hero.m_fitness < 40 || current_gen.m_gen == 0)
	{
		printf("\nGeneration %d. ", current_gen.m_gen + 1);
		current_gen.CreateNewGeneration(hero);
		current_gen.ComputeFitness(secret);
		current_gen.OrderPopulation();
		hero = current_gen.m_population[99];

		printf("HERO dna: ");
		hero.DisplayDNA();
		printf(", Fitness: %d\n", hero.m_fitness);
		
	}

	printf("\nSUCESS.\n");
	hero.DisplayEvolutionRecord(secret.m_dna);
	printf("\n");
	system("pause");

	return 0;
}