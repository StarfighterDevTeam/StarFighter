#ifndef GENERATION_H_INCLUDED
#define GENERATION_H_INCLUDED

#include "Individual.h"

class Generation
{
public:
	Generation();
	Generation(int gen_index);

	Individual m_population[POPULATION_SIZE];
	int m_gen;

	void OrderPopulation();
	void CreateNewGeneration(Individual& hero);
	void ComputeFitness(Individual& const secret);
	static void evoluate(Generation& gen);
	int getAverageFitness();
};

#endif // GENERATION_H_INCLUDED