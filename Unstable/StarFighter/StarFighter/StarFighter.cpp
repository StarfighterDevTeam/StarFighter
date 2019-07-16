#include "StarFighter.h"

static int ComputeFitness(Individual& individual, Individual& const secret)
{
	std::vector<int> slots_marked;

	individual.m_fitness = 0;
	for (int i = 0; i < 4; i++)//proposal
	{
		if (individual.m_dna[i] == secret.m_dna[i])
		{
			//"red" marker
			individual.m_fitness += 10;
			slots_marked.push_back(i);
			continue;
		}
	}

	for (int i = 0; i < 4; i++)//proposal
	{
		for (int j = 0; j < 4; j++)//secret
		{
			if (individual.m_dna[i] == secret.m_dna[j])
			{
				bool found = false;
				for (int v = 0; v < slots_marked.size(); v++)
				{
					if (slots_marked[v] == j)
					{
						found = true;
						break;
					}
				}

				if (found == false)
				{
					//"white" marker
					individual.m_fitness += 4;
					slots_marked.push_back(i);
					continue;
				}
			}
		}
	}

	return individual.m_fitness;
}

void ComputeFitness(Generation& const generation, Individual& const secret)
{
	for (int i = 0; i < 100; i++)
	{
		ComputeFitness(generation.m_population[i], secret);
	}
}

int main()
{
	printf("MASTERMIND - Genetic Algorithm\n\n");

	//Random seed
	srand(time(NULL));

	//randomize secret
	Individual secret;
	printf("secret: ");
	secret.DisplayDNA();

	printf("\nGeneration 0. ");
	Generation current_gen = Generation();
	ComputeFitness(current_gen, secret);
	current_gen.OrderPopulation();
	Individual& hero = current_gen.m_population[99];
	printf("HERO dna: ");
	hero.DisplayDNA();
	printf("Fitness: %d\n", hero.m_fitness);

	//Main loop
	while (hero.m_fitness < 40 || current_gen.m_gen == 0)
	{
		printf("\nGeneration %d. ", current_gen.m_gen + 1);
		current_gen.CreateNewGeneration(hero);
		ComputeFitness(current_gen, secret);
		current_gen.OrderPopulation();
		Individual& hero = current_gen.m_population[99];

		printf("HERO dna: ");
		hero.DisplayDNA();
		printf("Fitness: %d\n", hero.m_fitness);
	}

	return 0;
}