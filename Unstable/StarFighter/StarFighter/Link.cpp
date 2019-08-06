#include "Link.h"

extern Game* CurrentGame;

using namespace sf;

Link::Link(Node* node_a, Node* node_b) : LineObject(node_a->getPosition(), node_b->getPosition(), node_a->m_alliance)
{
	m_node_a = node_a;
	m_node_b = node_b;

	m_update_needed = true;
}

Link::~Link()
{

}

void Link::update(sf::Time deltaTime)
{
	if (m_node_a->m_hovered == true)
	{
		m_quad[0].color = sf::Color(255, 255, 255, 255);
		m_quad[3].color = sf::Color(255, 255, 255, 255);
	}
	else if (m_node_a->m_selected == true)
	{
		//m_quad[0].color = sf::Color(0, 255, 0, 255);
		//m_quad[3].color = sf::Color(0, 255, 0, 255);
	}
	else
	{
		m_quad[0].color = m_color;
		m_quad[3].color = m_color;
	}

	if (m_node_b->m_hovered == true)
	{
		m_quad[1].color = sf::Color(255, 255, 255, 255);
		m_quad[2].color = sf::Color(255, 255, 255, 255);
	}
	else if (m_node_b->m_selected == true)
	{
		//m_quad[1].color = sf::Color(0, 255, 0, 255);
		//m_quad[2].color = sf::Color(0, 255, 0, 255);
	}
	else
	{
		m_quad[1].color = m_color;
		m_quad[2].color = m_color;
	}

	//line display
	if (m_update_needed == true)
	{
		m_update_needed = false;
	
		m_points[0].position = m_node_a->getPosition();
		m_points[1].position = m_node_b->getPosition();

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
	}
}

void Link::ResetColor()
{
	for (int i = 0; i < 4; i++)
	{
		m_quad[i].color = m_color;
	}
}