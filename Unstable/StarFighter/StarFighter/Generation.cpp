#include "Generation.h"

Generation::Generation()
{
	Generation(0);
}

Generation::Generation(int gen_index)
{
	m_gen = gen_index;
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		m_population[i].m_index = i;
		m_population[i].m_gen = gen_index;
		m_population[i].m_generation = this;
	}
}

void Generation::OrderPopulation()
{
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		for (int j = i + 1; j < POPULATION_SIZE; j++)
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
	for (int i = POPULATION_SIZE - 1; i > 0; i--)
	{
		m_population[i].m_gen = m_gen;

		//evolution record
		Evolution* evolution = new Evolution();
		evolution->m_gen = m_gen;
		Individual::CopyDNA(m_population[i].m_dna, evolution->m_dna_output);
		m_population[i].m_evolution_record.push_back(evolution);

		//top 10% + hero
		if (i > POPULATION_SIZE - 10)
		{
			//do nothing
		}
		//10 random
		else if (i > POPULATION_SIZE - 20)
		{
			int random = RandomizeIntBetweenValues(0, i);
			Individual a;
			a.Copy(m_population[i]);
			m_population[i].Copy(m_population[random]);
			m_population[random].Copy(a);
		}
		//top 1 crossovers & mutations
		else if (i > POPULATION_SIZE - 50)
		{
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_FirstHalf);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_SecondHalf);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_AlternateOdd);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_AlternateNotOdd);

			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 1], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_FirstHalf);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_SecondHalf);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateOdd);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);
			Individual::Mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_FirstHalf);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_SecondHalf);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_AlternateOdd);
			Individual::CrossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);

			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 2], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 2], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Expand);

			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 4], Mutation_Add);
			Individual::Mutate(m_population[i--], m_population[POPULATION_SIZE - 4], Mutation_Expand);
		}
		else
		{
			m_population[i].RandomizeDNA();
		}
	}
}

void Generation::ComputeFitness(Individual& const secret)
{
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		Individual::ComputeFitness(m_population[i], secret);
	}
}