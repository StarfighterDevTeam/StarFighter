#include "LineObject.h"

LineObject::LineObject(sf::Vector2f position_a, sf::Vector2f position_b, AllianceType alliance)
{
	m_thickness = 3;
	m_alliance = alliance;
	m_color = GameObject::GetAllianceColor(alliance);
	ResetColor();
	m_garbageMe = false;
	m_update_needed = true;
}

LineObject::~LineObject()
{

}

void LineObject::UpdateQuadPointsPosition()
{
	//thickness
	sf::Vector2f direction = m_points[1].position - m_points[0].position;
	sf::Vector2f unitDirection = direction / std::sqrt(direction.x*direction.x + direction.y*direction.y);
	sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

	sf::Vector2f offset = (m_thickness / 2.f)*unitPerpendicular;

	m_quad[0].position = m_points[0].position + offset;
	m_quad[1].position = m_points[1].position + offset;
	m_quad[2].position = m_points[1].position - offset;
	m_quad[3].position = m_points[0].position - offset;
}

void LineObject::update(sf::Time deltaTime)
{
	
}

void LineObject::ResetColor()
{
	for (int i = 0; i < 4; i++)
	{
		m_quad[i].color = m_color;
	}
}