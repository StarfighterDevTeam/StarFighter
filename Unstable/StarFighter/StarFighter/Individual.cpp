#include "Individual.h"

Individual::Individual()
{
	m_generation = NULL;
	RandomizeDNA();

	//evolution record
	Evolution* evolution = new Evolution();
	evolution->m_gen = 0;
	Individual::CopyDNA(m_dna, evolution->m_dna_output);
	m_evolution_record.push_back(evolution);
}

Individual::~Individual()
{
	//evolution record
	for (int i = 0; i < m_evolution_record.size(); i++)
	{
		delete m_evolution_record[i];
	}
	m_evolution_record.clear();
}

void Individual::RandomizeDNA()
{
	for (int i = 0; i < 4; i++)
	{
		m_dna[i] = RandomizeIntBetweenValues(0, 7);
	}
}

void Individual::CopyDNA(int dna_input[], int dna_output[])
{
	for (int i = 0; i < 4; i++)
	{
		dna_output[i] = dna_input[i];
	}
}

void Individual::DisplayDNA()
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

void Individual::Copy(Individual& individual)
{
	m_index = individual.m_index;
	m_fitness = individual.m_fitness;
	m_gen = individual.m_gen;
	for (int i = 0; i < 4; i++)
	{
		m_dna[i] = individual.m_dna[i];
	}

	//evolution record
	
	for (int i = 0; i < m_evolution_record.size(); i++)
	{
		delete m_evolution_record[i];
	}
	m_evolution_record.clear();
	for (int i = 0; i < individual.m_evolution_record.size(); i++)
	{
		Evolution* evolution = new Evolution();
		evolution->Copy(*individual.m_evolution_record[i]);
		m_evolution_record.push_back(evolution);
	}
	
}

void Individual::CrossOver(Individual& output, Individual& input_a, Individual& input_b, CrossOverType type)
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

	//evolution record
	Evolution* evolution = new Evolution();
	evolution->m_crossover = (int)type;
	Individual::CopyDNA(input_a.m_dna, evolution->m_dna_input_a);
	Individual::CopyDNA(input_b.m_dna, evolution->m_dna_input_b);
	Individual::CopyDNA(output.m_dna, evolution->m_dna_output);
	output.m_evolution_record.push_back(evolution);
	
}

void Individual::Mutate(Individual& output, Individual& input, MutationType type)
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

	//evolution record
	Evolution* evolution = new Evolution();
	evolution->m_mutation = (int)type;
	Individual::CopyDNA(input.m_dna, evolution->m_dna_input_a);
	Individual::CopyDNA(output.m_dna, evolution->m_dna_output);
	output.m_evolution_record.push_back(evolution);
}