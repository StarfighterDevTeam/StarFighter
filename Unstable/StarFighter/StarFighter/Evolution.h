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
	void copy(Evolution& const evolution);

	int m_mutation;
	int m_crossover;
	int m_gen;
	uint8_t m_dna_input_a[DNA_LENGTH];
	uint8_t m_dna_input_b[DNA_LENGTH];
	uint8_t m_dna_output[DNA_LENGTH];
};

#endif // EVOLUTION_H_INCLUDED