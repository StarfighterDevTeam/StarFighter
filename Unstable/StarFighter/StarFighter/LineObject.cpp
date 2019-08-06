#include "LineObject.h"

LineObject::LineObject(sf::Vector2f position_a, sf::Vector2f position_b, AllianceType alliance)
{
	m_thickness = 3;
	m_alliance = alliance;
	m_color = GameObject::GetAllianceColor(alliance);
	m_garbageMe = false;
}

LineObject::~LineObject()
{

}

void LineObject::update(sf::Time deltaTime)
{
	
}