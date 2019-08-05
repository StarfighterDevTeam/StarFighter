#include "LineObject.h"

LineObject::LineObject(sf::Vector2f position_a, sf::Vector2f position_b, AllianceType alliance)
{
	m_points[0].position = position_a;
	m_points[0].color = alliance == PlayerAlliance ? sf::Color::Blue : sf::Color::Red;

	m_points[1].position = position_b;
	m_points[1].color = alliance == PlayerAlliance ? sf::Color::Blue : sf::Color::Red;

	m_alliance = alliance;
}

LineObject::~LineObject()
{

}

void LineObject::update(sf::Time deltaTime)
{
	
}