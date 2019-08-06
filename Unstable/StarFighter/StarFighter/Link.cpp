#include "Link.h"

extern Game* CurrentGame;

using namespace sf;

Link::Link(Node* node_a, Node* node_b) : LineObject(node_a->getPosition(), node_b->getPosition(), node_a->m_alliance)
{
	m_node_a = node_a;
	m_node_b = node_b;

	m_update_needed = true;
}

void Link::update(sf::Time deltaTime)
{
	//line display
	if (m_update_needed == true)
	{
		m_update_needed = false;
	
		m_points[0].position = m_node_a->getPosition();
		m_points[0].color = m_alliance == PlayerAlliance ? sf::Color::Blue : sf::Color::Red;

		m_points[1].position = m_node_b->getPosition();
		m_points[1].color = m_alliance == PlayerAlliance ? sf::Color::Blue : sf::Color::Red;

		//thickness
		sf::Vector2f direction = m_points[1].position - m_points[0].position;
		sf::Vector2f unitDirection = direction / std::sqrt(direction.x*direction.x + direction.y*direction.y);
		sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (m_thickness / 2.f)*unitPerpendicular;

		sf::Vector2f radius_vector_a = direction;
		sf::Vector2f radius_vector_b = direction;
		ScaleVector(&radius_vector_a, m_node_a->getRadius());
		ScaleVector(&radius_vector_b, m_node_b->getRadius());

		m_quad[0].position = m_points[0].position + offset + radius_vector_a;
		m_quad[1].position = m_points[1].position + offset - radius_vector_b;
		m_quad[2].position = m_points[1].position - offset - radius_vector_b;
		m_quad[3].position = m_points[0].position - offset + radius_vector_a;
		for (int i = 0; i < 4; i++)
		{
			m_quad[i].color = m_points[0].color;
		}
	}
}

void Link::ResetColor()
{
	m_points[0].color = sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE);
	m_points[1].color = sf::Color(m_color.r, m_color.g, m_color.b, 255);
}