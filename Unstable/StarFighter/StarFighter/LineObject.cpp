#include "LineObject.h"

LineObject::LineObject(sf::Vector2f position_a, sf::Vector2f position_b, AllianceType alliance)
{
	m_thickness = 3;
	m_alliance = alliance;
}

LineObject::~LineObject()
{

}

void LineObject::update(sf::Time deltaTime)
{
	
}