#include "Neuron.h"

Layer::Layer(int nb_neuron, LayerType type)
{
	for (int i = 0; i < nb_neuron; i++)
	{
		m_neurons.push_back(Neuron());
	}
	m_nb_neurons = nb_neuron;

	if (type != OutpuLayer)//bias neuron
	{
		m_neurons.push_back(Neuron());
		m_neurons.back().m_value = 1.f;
		m_nb_neurons++;
	}

	m_type = type;
}