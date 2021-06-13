#include "Neuron.h"

Neuron::Neuron()
{
	m_is_bias = false;

	//m_circle.setOutlineColor(sf::Color::Red);
	m_circle.setOutlineThickness(0);
	m_circle.setFillColor(sf::Color::Red);
	m_circle.setRadius(10);
	m_circle.setOrigin(sf::Vector2f(0, 0));
}

Layer::Layer(int nb_neuron, LayerType type, bool use_bias)
{
	for (int i = 0; i < nb_neuron; i++)
	{
		Neuron* neuron = new Neuron();
		m_neurons.push_back(neuron);
	}
		
	m_nb_neurons = nb_neuron;

	if (type != OutpuLayer && use_bias == true)//bias neuron
	{
		Neuron* neuron = new Neuron();
		m_neurons.push_back(neuron);
		neuron->m_value = 1.f;
		neuron->m_is_bias = true;
		m_nb_neurons++;
	}

	m_type = type;
}

Layer::~Layer()
{
	for (Neuron* neuron : m_neurons)
		delete neuron;
}