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

	static void copyDNA(const uint8_t dna_input[], uint8_t dna_output[]);
	void displayDNA();
	static void displayDNA(uint8_t dna[]);
	void DisplayEvolutionRecord(uint8_t dna_secret[]);
	void copy(const Individual& individual);
	static void crossOver(Individual& output, const Individual& input_a, const Individual& input_b, CrossOverType type);
	static void mutate(Individual& output, const Individual& input, MutationType type, int repeat);

	static int ComputeFitness(Individual& individual, Individual& const secret);
	static int ComputeFitness(uint8_t dna_individual[], uint8_t dna_secret[]);
	void setFitness(const int score)		{ m_fitness = score; }
	int getFitness() const					{ return m_fitness; }
	void setBadGene(int badGeneId)			{ m_badGene = badGeneId; }
	int getBadGene() const					{ return m_badGene; }

	bool saveInFile();
	bool loadFromFile();

	uint8_t m_dna[DNA_LENGTH];
	int m_index;
	int m_fitness;
	int m_gen;
	Generation* m_generation;
	int m_badGene;

	std::vector<Evolution*> m_evolution_record;
};

#endif // INDIVIDUAL_H_INCLUDED