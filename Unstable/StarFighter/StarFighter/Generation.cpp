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
				a.copy(m_population[i]);
				m_population[i].copy(m_population[j]);
				m_population[j].copy(a);
			}
		}
	}
}

/*
void Generation::CreateNewGeneration(Individual& hero)
{	
	m_gen++;
	for (int i = POPULATION_SIZE - 1; i > 0; i--)
	{
		m_population[i].m_gen = m_gen;

		//evolution record
		//Evolution* evolution = new Evolution();
		//evolution->m_gen = m_gen;
		//Individual::copyDNA(m_population[i].m_dna, evolution->m_dna_output);
		//m_population[i].m_evolution_record.push_back(evolution);

		//top 30%
		if (i > POPULATION_SIZE * (1 - 0.3f))
		{
			//do nothing
		}
		//random 20%
		else if (i > POPULATION_SIZE - 20)
		{
			int random = RandomizeIntBetweenValues(0, i);
			Individual a;
			a.copy(m_population[i]);
			m_population[i].copy(m_population[random]);
			m_population[random].copy(a);
		}
		//top 1 crossovers & mutations
		else if (i > POPULATION_SIZE - 50)
		{
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_FirstHalf);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_SecondHalf);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_AlternateOdd);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 2], CrossOver_AlternateNotOdd);

			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 1], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 1], Mutation_Expand);

			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_FirstHalf);
			Individual::mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_SecondHalf);
			Individual::mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateOdd);
			Individual::mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);
			Individual::mutate(m_population[i--], m_population[i + 1], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[i + 2], Mutation_Expand);

			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_FirstHalf);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_SecondHalf);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 4], CrossOver_AlternateOdd);
			Individual::crossOver(m_population[i--], m_population[POPULATION_SIZE - 1], m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);

			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 2], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 2], Mutation_Expand);

			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Expand);

			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 3], Mutation_Expand);

			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 4], Mutation_Add);
			Individual::mutate(m_population[i--], m_population[POPULATION_SIZE - 4], Mutation_Expand);
		}
		else
		{
			m_population[i].randomizeDNA();
		}
	}
}
*/

void Generation::ComputeFitness(Individual& const secret)
{
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		Individual::ComputeFitness(m_population[i], secret);
	}
}

void Generation::evoluate(const Individual& hero)
{
	Generation oldGen = *this;//make a copy before overwriting gen

	m_gen++;
	int last_threshold = POPULATION_SIZE - 1;
	for (int i = POPULATION_SIZE - 1; i > 0; i--)
	{
		m_population[i].m_gen = m_gen;

		//evolution record
		//Evolution* evolution = new Evolution();
		//evolution->m_gen = m_gen;
		//Individual::copyDNA(m_population[i].m_dna, evolution->m_dna_output);
		//m_population[i].m_evolution_record.push_back(evolution);

		//hero: keep him
		if (i > POPULATION_SIZE - 1)
		{
			Individual::copyDNA(hero.m_dna, m_population[i].m_dna);
		}
		//top 9: keep them
		else if (i > POPULATION_SIZE - 1 - 9)
		{
			//do nothing
		}
		//random 3: keep them
		else if (i > POPULATION_SIZE - 1 - 9 - 3)
		{
			const int random = RandomizeIntBetweenValues(0, i);
			Individual::copyDNA(oldGen.m_population[random].m_dna, m_population[i].m_dna);
		}
		//new random 5: generate them
		else if (i > POPULATION_SIZE - 1 - 9 - 3 - 5)
		{
			m_population[i].randomizeDNA();
		}
		//last 12 : crossovers & mutations between top 3 DNAs
		else 
		{
			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], hero, CrossOver_FirstHalf);
			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], hero, CrossOver_SecondHalf);
			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], hero, CrossOver_AlternateOdd);
			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], hero, CrossOver_AlternateNotOdd);

			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 2], CrossOver_FirstHalf);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 1], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 2], Mutation_Expand);

			Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 2], CrossOver_SecondHalf);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 1], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 2], Mutation_Expand);

			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 3], CrossOver_AlternateOdd);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 1], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 2], Mutation_Expand);

			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 1], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[i + 2], Mutation_Expand);

			//Individual::crossOver(m_population[i--], oldGen.m_population[POPULATION_SIZE - 2], oldGen.m_population[POPULATION_SIZE - 3], CrossOver_AlternateOdd);

			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 4], CrossOver_FirstHalf);
			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 4], CrossOver_SecondHalf);
			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 4], CrossOver_AlternateOdd);
			//Individual::crossOver(	m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], oldGen.m_population[POPULATION_SIZE - 3], CrossOver_AlternateNotOdd);

			Individual::mutate(		m_population[i--], hero, Mutation_Add, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));
			Individual::mutate(		m_population[i--], hero, Mutation_Expand, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));

			Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], Mutation_Add, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));
			Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 1], Mutation_Expand, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));

			Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 2], Mutation_Add, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));
			Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 2], Mutation_Expand, RandomizeIntBetweenValues(1, (int)(DNA_LENGTH * 0.01f)));

			//Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 3], Mutation_Add);
			//Individual::mutate(m_population[i--], oldGen.m_population[POPULATION_SIZE - 3], Mutation_Expand);

			//Individual::mutate(		m_population[i--], oldGen.m_population[POPULATION_SIZE - 3], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[POPULATION_SIZE - 3], Mutation_Expand);
			//
			//Individual::mutate(		m_population[i--], oldGen.m_population[POPULATION_SIZE - 4], Mutation_Add);
			//Individual::mutate(		m_population[i--], oldGen.m_population[POPULATION_SIZE - 4], Mutation_Expand);
		}
	}
}

int Generation::getAverageFitness()
{
	int sum = 0;
	for (int i = 0; i < POPULATION_SIZE - 1; i++)
		sum += m_population[i].getFitness();

	sum /= (POPULATION_SIZE - 1);
	return sum;
}