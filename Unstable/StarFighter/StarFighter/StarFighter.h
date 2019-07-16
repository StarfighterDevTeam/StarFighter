#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <SFML/Audio.hpp>
#include "Globals.h"

enum CrossOverType
{
	CrossOver_FirstHalf,
	CrossOver_SecondHalf,
	CrossOver_AlternateOdd,
	CrossOver_AlternateNotOdd,
};

enum MutationType
{
	Mutation_Add,
	Mutation_Erase,
	Mutation_Expand,
};

class Individual
{
public:
	Individual()
	{
		RandomizeDNA();
	};

	void RandomizeDNA()
	{
		for (int i = 0; i < 4; i++)
		{
			m_dna[i] = RandomizeIntBetweenValues(0, 7);
		}
	}

	void DisplayDNA()
	{
		for (int i = 0; i < 4; i++)
		{
			printf(" %d", m_dna[i]);
			if (i < 4 - 1)
				printf(",");
			else
				printf("\n");
		}
	};
	void Copy(Individual& individual)
	{
		m_index = individual.m_index;
		m_fitness = individual.m_fitness;
		m_gen = individual.m_gen;
		for (int i = 0; i < 4; i++)
		{
			m_dna[i] = individual.m_dna[i];
		}
	}
	static void CrossOver(Individual& output, Individual& input_a, Individual& input_b, CrossOverType type)
	{
		switch (type)
			{
			case CrossOver_FirstHalf:
			{
				for (int i = 0; i < 4; i++)
				{
					if (i < 4 / 2)
						output.m_dna[i] = input_b.m_dna[i];
					else
						output.m_dna[i] = input_a.m_dna[i];
				}
				break;
			}
			case CrossOver_SecondHalf:
			{
				for (int i = 0; i < 4; i++)
				{
					if (i < 4 / 2)
						output.m_dna[i] = input_a.m_dna[i];
					else
						output.m_dna[i] = input_b.m_dna[i];
				}
				break;
			}
			case CrossOver_AlternateOdd:
			{
				for (int i = 0; i < 4; i++)
				{
					if (i % 2 == 0)
						output.m_dna[i] = input_a.m_dna[i];
					else
						output.m_dna[i] = input_b.m_dna[i];
				}
				break;
			}
			case CrossOver_AlternateNotOdd:
			{
				for (int i = 0; i < 4; i++)
				{
					if (i % 2 == 0)
						output.m_dna[i] = input_b.m_dna[i];
					else
						output.m_dna[i] = input_a.m_dna[i];
				}
				break;
			}
		default:
			break;
		}
	}
	static void Mutate(Individual& output, Individual& input, MutationType type)
	{
		int r = RandomizeIntBetweenValues(0, 3);
		int v = RandomizeIntBetweenValues(0, 7);

		switch (type)
		{
			case Mutation_Add:
			{
				output.m_dna[r] = (input.m_dna[r] + v) % 8;
				break;
			}
			case Mutation_Erase:
			{
				output.m_dna[r] = 0;
				break;
			}
			case Mutation_Expand:
			{
				output.m_dna[(r + v) % 8] = input.m_dna[r];
				break;
			}
			default:
				break;
		}
	}
	
	int m_dna[4];
	int m_index;
	int m_fitness;
	int m_gen;
	int m_mutations;
	int m_crossovers;
	int m_startovers;
};

class Generation
{
public:
	Generation()
	{ 
		m_gen = 0;
		for (int i = 0; i < 100; i++)
		{
			m_population[i].m_index = i;
			m_population[i].m_gen = 0;
		}
	};
	
	Individual m_population[100];
	int m_gen;

	void OrderPopulation()
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

	void CreateNewGeneration(Individual& hero)
	{
		m_gen++;
		for (int i = 99; i > 0; i--)
		{
			m_population[i].m_gen++;

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
				Individual::Mutate(m_population[i--], m_population[i+1], Mutation_Add);
				Individual::Mutate(m_population[i--], m_population[i+2], Mutation_Expand);

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
};


#endif // STARFIGHTER_H_INCLUDED