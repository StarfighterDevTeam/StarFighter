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
		printf("%d", m_dna[i]);
		if (i < 4 - 1)
			printf(",");
	}
};

void Individual::DisplayDNA(int dna[])
{
	for (int i = 0; i < 4; i++)
	{
		printf("%d", dna[i]);
		if (i < 4 - 1)
			printf(",");
	}
};

void Individual::DisplayEvolutionRecord(int dna_secret[])
{
	printf("Evolution record:\n");
	for (int i = 0; i < m_evolution_record.size(); i++)
	{
		Evolution& evo = *m_evolution_record[i];
		
		if (evo.m_gen > -1)
		{
			printf("-Generation: %d", evo.m_gen);
			printf(" -> ");
			DisplayDNA();
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}
		
		if (evo.m_mutation > -1)
		{
			printf("	-Mutation: ");
			switch (evo.m_mutation)
			{
				case Mutation_Add:
				{
					printf("Add.");
					break;
				}
				case Mutation_Erase:
				{
					printf("Erase.");
					break;
				}
				case Mutation_Expand:
				{
					printf("Expand.");
					break;
				}
			}

			printf(" ");
			DisplayDNA(evo.m_dna_input_a);
			printf(" -> ");
			DisplayDNA(evo.m_dna_output);
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}

		if (evo.m_crossover > -1)
		{
			printf("	-Crossover: ");
			switch (evo.m_crossover)
			{
				case CrossOver_FirstHalf:
				{
					printf("First half.");
					break;
				}
				case CrossOver_SecondHalf:
				{
					printf("Second half.");
					break;
				}
				case CrossOver_AlternateOdd:
				{
					printf("Alternate odd.");
					break;
				}
				case CrossOver_AlternateNotOdd:
				{
					printf("Alternate not odd.");
					break;
				}
			}

			printf(" ");
			DisplayDNA(evo.m_dna_input_a);
			printf(" + ");
			DisplayDNA(evo.m_dna_input_b);
			printf(" -> ");
			DisplayDNA(evo.m_dna_output);
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}
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
	output.Copy(input_a);

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
	output.Copy(input);

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

int Individual::ComputeFitness(int dna_individual[], int dna_secret[])
{
	std::vector<int> slots_marked;

	int fitness = 0;
	for (int i = 0; i < 4; i++)//proposal
	{
		if (dna_individual[i] == dna_secret[i])
		{
			//"red" marker
			fitness += 10;
			slots_marked.push_back(i);
			continue;
		}
	}

	for (int i = 0; i < 4; i++)//proposal
	{
		for (int j = 0; j < 4; j++)//secret
		{
			if (dna_individual[i] == dna_secret[j])
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
					fitness += 4;
					slots_marked.push_back(i);
					continue;
				}
			}
		}
	}

	return fitness;
}


int Individual::ComputeFitness(Individual& individual, Individual& const secret)
{
	individual.m_fitness = ComputeFitness(individual.m_dna, secret.m_dna);

	return individual.m_fitness;
}