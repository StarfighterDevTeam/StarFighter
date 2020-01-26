#ifndef CHROMOSOME_H_INCLUDED
#define CHROMOSOME_H_INCLUDED

#include "Ship.h"

class Chromosome
{
public:
	Chromosome();
	~Chromosome();

	float m_dna[38];

	void UpdateDNA(Wing* wing, Node* finish);

	void MagicFunction(float& output_direction_x, float& output_fire);
};

#endif // CHROMOSOME_H_INCLUDED
