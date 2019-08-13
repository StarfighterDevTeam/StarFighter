#include "Node.h"

extern Game* CurrentGame;

using namespace sf;

Node::Node(sf::Vector2f position, AllianceType alliance, float radius) : L16Entity(position, alliance, radius)
{
	m_type = L16Entity_Node;
	m_is_terminal_node = false;

	//m_visible = true;// alliance == PlayerAlliance;
}

Node::~Node()
{

}

void Node::update(sf::Time deltaTime)
{
	//radar
	if (m_is_terminal_node == true)
	{
		m_radar_activated = true;
	}
	else
	{
		m_radar_activated = false;
		for (vector<L16Entity*>::iterator it = m_linked_entities.begin(); it != m_linked_entities.end(); it++)
		{
			if ((*it)->m_radar_activated == true)
			{
				m_radar_activated = true;
				break;
			}
		}
	}

	//hovering & selection
	m_hovered = false;
	if (IsHoveredByMouse() == true)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_entity = this;

		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick && m_alliance == PlayerAlliance)
		{
			if ((*CurrentGame).m_selected_entity != NULL)
			{
				(*CurrentGame).m_selected_entity->m_selected = false;
				(*CurrentGame).m_selected_entity->ResetColor();
			}

			m_selected = true;
			(*CurrentGame).m_selected_entity = this;
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

	L16Entity::update(deltaTime);
}

Wing* Node::CreateWing()
{
	Wing* wing = new Wing(getPosition(), m_alliance);
	(*CurrentGame).AddCircleObject(wing, Circle_L16Entity);
	(*CurrentGame).m_wings.push_back(wing);

	return wing;
}