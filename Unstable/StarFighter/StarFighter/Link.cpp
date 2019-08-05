#include "Link.h"

extern Game* CurrentGame;

using namespace sf;

Link::Link(Node* node_a, Node* node_b) : LineObject(node_a->getPosition(), node_b->getPosition(), node_a->m_alliance)
{
	m_node_a = node_a;
	m_node_b = node_b;
}

void Link::update(sf::Time deltaTime)
{

}

void Link::ResetColor()
{
	m_points[0].color = sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE);
	m_points[1].color = sf::Color(m_color.r, m_color.g, m_color.b, 255);
}