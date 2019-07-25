#include "Node.h"

extern Game* CurrentGame;

using namespace sf;

Node::Node(sf::Vector2f position, bool player, float radius) : GameObject(position, sf::Vector2f(0, 0), player ? sf::Color::Blue : sf::Color::Red, radius, 4)
{
	m_hovered = false;
	m_selected = false;
}

Node::Node(sf::Vector2f position, bool player) : Node::Node(position, player, 16)
{

}

void Node::update(sf::Time deltaTime)
{
	if (IsHoveredByMouse() == true)
	{
		setColor(sf::Color(m_color.r, m_color.g, m_color.b, 120));
		m_hovered = true;
		(*CurrentGame).m_hovered_node = this;

		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			if ((*CurrentGame).m_selected_node != NULL && (*CurrentGame).m_selected_node != this)
			{
				(*CurrentGame).m_selected_node->m_selected = false;
			}

			if ((*CurrentGame).m_selected_node != this)
			{
				m_selected = true;
				(*CurrentGame).m_selected_node = this;
			}
		}
	}
	else
	{
		m_hovered = false;
		setColor(sf::Color(m_color.r, m_color.g, m_color.b, 255));
	}

	GameObject::update(deltaTime);
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
	if (dx < m_size.x * 0.5 && dy < m_size.y * 0.5)
	{
		return true;
	}
	else
	{
		return false;
	}
}