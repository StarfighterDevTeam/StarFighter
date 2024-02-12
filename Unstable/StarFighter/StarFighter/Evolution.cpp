#include "Evolution.h"

Evolution::Evolution()
{
	m_mutation = -1;
	m_crossover = -1;
	m_gen = -1;
	for (short i = 0; i < DNA_LENGTH; i++)
	{
		m_dna_input_a[i] = -1;
		m_dna_input_b[i] = -1;
		m_dna_output[i] = -1;
	}
}

void Evolution::copy(Evolution& const evolution)
{
	m_mutation = evolution.m_mutation;
	m_crossover = evolution.m_crossover;
	m_gen = evolution.m_gen;
	for (short i = 0; i < DNA_LENGTH; i++)
	{
		m_dna_input_a[i] = evolution.m_dna_input_a[i];
		m_dna_input_b[i] = evolution.m_dna_input_b[i];
		m_dna_output[i] = evolution.m_dna_output[i];
	}
}