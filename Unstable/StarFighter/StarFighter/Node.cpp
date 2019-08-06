#include "Node.h"

extern Game* CurrentGame;

using namespace sf;

Node::Node(sf::Vector2f position, AllianceType alliance, float radius) : CircleObject(alliance)
{
	m_hovered = false;
	m_selected = false;
	setPosition(position);
	setOrigin(sf::Vector2f(radius, radius));
	setRadius(radius);
	m_color = alliance == PlayerAlliance ? sf::Color(0, 0, 255, 255) : sf::Color(255, 0, 0, 255);
	setFillColor(alliance == PlayerAlliance ? sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE) : sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE));
	setOutlineColor(alliance == PlayerAlliance ? sf::Color(m_color.r, m_color.g, m_color.b, 255) : sf::Color(m_color.r, m_color.g, m_color.b, 255));
	setOutlineThickness(-4);
}

Node::Node(sf::Vector2f position, AllianceType alliance) : Node::Node(position, alliance, 16)
{

}

void Node::update(sf::Time deltaTime)
{
	m_hovered = false;
	if (IsHoveredByMouse() == true)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_node = this;

		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			if ((*CurrentGame).m_selected_node != NULL)
			{
				(*CurrentGame).m_selected_node->m_selected = false;
				(*CurrentGame).m_selected_node->ResetColor();
			}

			m_selected = true;
			(*CurrentGame).m_selected_node = this;
		}
	}

	ResetColor();
	if (m_selected == true)
	{
		setFillColor(sf::Color(0, 255, 0, GHOST_ALPHA_VALUE));
		setOutlineColor(sf::Color(0, 255, 0, 255));
	}
	if (m_hovered == true)
	{
		setOutlineColor(sf::Color(255, 255, 255, 255));
	}

	CircleObject::update(deltaTime);
}

bool Node::IsHoveredByMouse()
{
	if ((*CurrentGame).m_window_has_focus == false)
	{
		return false;
	}

	//collision with a circle = inside radius?
	float dx = abs((*CurrentGame).m_mouse_pos.x - getPosition().x);
	float dy = abs((*CurrentGame).m_mouse_pos.y - getPosition().y);
	if (dx < getRadius() && dy < getRadius())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Node::ResetColor()
{
	setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE));
	setOutlineColor(sf::Color(m_color.r, m_color.g, m_color.b, 255));
}