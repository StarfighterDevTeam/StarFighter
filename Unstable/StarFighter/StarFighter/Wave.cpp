#include "Wave.h"

extern Game* CurrentGame;

using namespace sf;

Wave::Wave(sf::Vector2f position, AllianceType alliance, float radius, float expansion_speed) : CircleObject(alliance)
{
	setRadius(radius);
	setOrigin(sf::Vector2f(radius, radius));
	setPosition(position);
	m_expansion_speed = expansion_speed;

	m_color = alliance == PlayerAlliance ? sf::Color(0, 0, 255, 255) : sf::Color(255, 0, 0, 255);
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
}