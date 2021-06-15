#include "Neuron.h"

extern Resources* GlobalResources;

Neuron::Neuron()
{
	m_is_bias = false;

	m_circle.setOutlineThickness(0);
	m_circle.setFillColor(sf::Color::Red);
	//m_circle.setOutlineThickness(2);
	//m_circle.setOutlineColor(sf::Color::Red);
	m_circle.setOrigin(sf::Vector2f(0, 0));

	sf::Text m_input_text;
	sf::Text m_output_text;
	vector <sf::Text> m_weight_texts;

	m_input_text.setCharacterSize(10);
	m_input_text.setColor(sf::Color::Black);
	m_input_text.setOrigin(sf::Vector2f(0, 0));
	m_input_text.setFont(*GlobalResources->m_font);

	m_output_text.setCharacterSize(10);
	m_output_text.setColor(sf::Color::Black);
	m_output_text.setOrigin(sf::Vector2f(0, 0));
	m_output_text.setFont(*GlobalResources->m_font);
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

bool Neuron::IsHovered(sf::Vector2f mouse_position)
{
	return (mouse_position.x - (m_circle.getPosition().x + m_circle.getRadius())) * (mouse_position.x - (m_circle.getPosition().x + m_circle.getRadius())) + (mouse_position.y - (m_circle.getPosition().y + m_circle.getRadius())) * (mouse_position.y - (m_circle.getPosition().y + m_circle.getRadius())) < m_circle.getRadius() * m_circle.getRadius();
}

Layer::~Layer()
{
	for (Neuron* neuron : m_neurons)
		delete neuron;
}