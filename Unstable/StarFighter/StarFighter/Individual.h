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

	void randomizeDNA();

	static void copyDNA(int dna_input[], int dna_output[]);
	void DisplayDNA();
	static void DisplayDNA(int dna[]);
	void DisplayEvolutionRecord(int dna_secret[]);
	void copy(Individual& individual);
	static void crossOver(Individual& output, Individual& input_a, Individual& input_b, CrossOverType type);
	static void mutate(Individual& output, Individual& input, MutationType type);

	static int ComputeFitness(Individual& individual, Individual& const secret);
	static int ComputeFitness(int dna_individual[], int dna_secret[]);
	void setFitness(const int score)		{ m_fitness = score; }
	int getFitness()						{ return m_fitness; }

	int m_dna[DNA_LENGTH];
	int m_index;
	int m_fitness;
	int m_gen;
	Generation* m_generation;

	std::vector<Evolution*> m_evolution_record;
};

#endif // INDIVIDUAL_H_INCLUDED