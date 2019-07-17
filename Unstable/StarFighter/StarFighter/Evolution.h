#ifndef EVOLUTION_H_INCLUDED
#define EVOLUTION_H_INCLUDED

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

struct Evolution
{
	Evolution();
	void Copy(Evolution& const evolution);

	int m_mutation;
	int m_crossover;
	int m_gen;
	int m_dna_input_a[4];
	int m_dna_input_b[4];
	int m_dna_output[4];
};

#endif // EVOLUTION_H_INCLUDED