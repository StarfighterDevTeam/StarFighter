#include "Star.h"

extern Game* CurrentGame;

using namespace sf;

Star::Star(sf::Vector2f position, sf::Vector2f speed, sf::Color color, sf::Vector2f size, float pulse_periodicity) : GameObject(position, speed, color, size)
{
	m_pulse_periodicity = pulse_periodicity;
	m_pulse_clock = 0.f;
	m_color = color;
}

void Star::Update(sf::Time deltaTime)
{
	//pulse
	if (m_pulse_periodicity > 0)
	{
		m_pulse_clock += deltaTime.asSeconds();
		if (m_pulse_clock > m_pulse_periodicity)
		{
			m_pulse_clock -= m_pulse_periodicity;
		}
		
		setColor(sf::Color(255, 255, 255, (sf::Uint8)(m_color.a * (1.f - 0.20f * cos(m_pulse_clock * 2.f * M_PI_F / m_pulse_periodicity)))));
	}

	GameObject::Update(deltaTime);
}