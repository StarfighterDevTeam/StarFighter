#include "Wave.h"

extern Game* CurrentGame;

using namespace sf;

Wave::Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed, float lifespan) : CircleObject(alliance)
{
	setRadius(radius);
	setOrigin(sf::Vector2f(radius, radius));
	setPosition(position);
	m_expansion_speed = expansion_speed;
	m_lifespan = lifespan;

	setFillColor(sf::Color(0, 0, 0, 0));
	setOutlineColor(m_color);
	setOutlineThickness(4);
}

Wave::~Wave()
{

}

void Wave::update(sf::Time deltaTime)
{
	setRadius(getRadius() + m_expansion_speed * deltaTime.asSeconds());
	setOrigin(sf::Vector2f(getRadius(), getRadius()));

	m_lifespan -= deltaTime.asSeconds();

	if (m_lifespan < 1)
	{
		Uint8 alpha = (Uint8)(m_lifespan * 255);
		setOutlineColor(sf::Color(m_color.r, m_color.g, m_color.b, alpha));
	}

	if (m_lifespan <= 0)
	{
		m_garbageMe = true;
	}
}


Wave* Wave::CreateWaveBounce(sf::Vector2f position, float radius, sf::Vector2f vector)
{
	Wave* wave = new Wave(position, NeutralAlliance, radius, m_expansion_speed, m_lifespan);
	(*CurrentGame).AddCircleObject(wave, WaveType);

	m_lifespan = 0;

	return wave;
}

void Wave::WaveReception()
{
	m_lifespan = 0;
}