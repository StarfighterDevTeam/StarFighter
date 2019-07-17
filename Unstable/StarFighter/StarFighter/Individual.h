#ifndef INDIVIDUAL_H_INCLUDED
#define INDIVIDUAL_H_INCLUDED

#include "Globals.h"
#include "Evolution.h"

class Generation;
class Individual
{
public:
	Individual();
	~Individual();

	void RandomizeDNA();

	static void CopyDNA(int dna_input[], int dna_output[]);
	void DisplayDNA();
	void Copy(Individual& individual);
	static void CrossOver(Individual& output, Individual& input_a, Individual& input_b, CrossOverType type);
	static void Mutate(Individual& output, Individual& input, MutationType type);

	int m_dna[4];
	int m_index;
	int m_fitness;
	int m_gen;
	Generation* m_generation;

	std::vector<Evolution*> m_evolution_record;
};

#endif // INDIVIDUAL_H_INCLUDED