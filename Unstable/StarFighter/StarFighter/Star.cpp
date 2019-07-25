#include "Star.h"

extern Game* CurrentGame;

using namespace sf;

Star::Star(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float pulse_periodicity) : GameObject(position, speed, color, size)
{
	m_pulse_periodicity = pulse_periodicity;
	m_pulse_clock = 0.f;
	m_color = color;
}

void Star::update(sf::Time deltaTime)
{
	//pulse
	if (m_pulse_periodicity > 0)
	{
		m_pulse_clock += deltaTime.asSeconds();
		if (m_pulse_clock > m_pulse_periodicity)
		{
			m_pulse_clock -= m_pulse_periodicity;
		}
		
		setColor(sf::Color(255, 255, 255, m_color.a * (1 - 0.20 * cos(m_pulse_clock * 2 * M_PI / m_pulse_periodicity))));
	}

	GameObject::update(deltaTime);
}