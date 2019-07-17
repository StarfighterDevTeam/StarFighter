#include "Generation.h"

Generation::Generation()
{
	m_gen = 0;
	for (int i = 0; i < 100; i++)
	{
		m_population[i].m_index = i;
		m_population[i].m_gen = 0;
		m_population[i].m_generation = this;
	}
}

void Generation::OrderPopulation()
{
	for (int i = 0; i < 100; i++)
	{
		for (int j = i + 1; j < 100; j++)
		{
			if (m_population[i].m_fitness > m_population[j].m_fitness)
			{
				Individual a;
				a.Copy(m_population[i]);
				m_population[i].Copy(m_population[j]);
				m_population[j].Copy(a);
			}
		}
	}
}

void Generation::CreateNewGeneration(Individual& hero)
{	
	m_gen++;
	for (int i = 99; i > 0; i--)
	{
		m_population[i].m_gen = m_gen;

		//evolution record
		Evolution* evolution = new Evolution();
		evolution->m_gen = m_gen;
		m_population[i].m_evolution_record.push_back(evolution);

		//top 10% + hero
		if (i > 90)
		{
			//do nothing
		}
		else if (i > 89)
		{
			//reincarnate hero from previous gen if it was better than our top10
			if (hero.m_fitness > m_population[i].m_fitness)
			{
				m_population[i].Copy(hero);
			}
		}
		//10 random
		else if (i > 79)
		{
			int random = RandomizeIntBetweenValues(0, i);
			Individual a;
			a.Copy(m_population[i]);
			m_population[i].Copy(m_population[random]);
			m_population[random].Copy(a);
		}
		//top 1 crossovers & mutations
		else if (i > 49)
		{
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 1], CrossOver_FirstHalf);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 1], CrossOver_SecondHalf);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 1], CrossOver_AlternateOdd);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 1], CrossOver_AlternateNotOdd);

			Individual::Mutate(m_population[i--], m_population[99], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[99], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 2], CrossOver_FirstHalf);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 2], CrossOver_SecondHalf);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 2], CrossOver_AlternateOdd);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 2], CrossOver_AlternateNotOdd);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 3], CrossOver_FirstHalf);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 3], CrossOver_SecondHalf);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 3], CrossOver_AlternateOdd);
			Individual::CrossOver(m_population[i--], m_population[99], m_population[99 - 2], CrossOver_AlternateNotOdd);

			Individual::Mutate(m_population[i--], m_population[99 - 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[99 - 1], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[99 - 2], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[99 - 2], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[99 - 2], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[99 - 2], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[99 - 3], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[99 - 3], Mutation_Expand);
		}
		else
		{
			m_population[i].RandomizeDNA();
		}
	}
}

int Generation::ComputeFitness(Individual& individual, Individual& const secret)
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

void Generation::ComputeFitness(Individual& const secret)
{
	for (int i = 0; i < 100; i++)
	{
		ComputeFitness(m_population[i], secret);
	}
}