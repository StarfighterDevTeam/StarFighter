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
	setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE));
	setOutlineColor(m_color);
	setOutlineThickness(-4);

	m_radar_activated = false;
	m_radar_frequency = 1.f;
	m_radar_frequency_clock = 0;
	m_radar_range = 100;
	m_radar_speed = 50;
	m_radar_direction = 0;
	m_radar_coverage = 60;

	m_is_terminal_node = false;
	m_ghost = false;
}

Node::Node(sf::Vector2f position, AllianceType alliance) : Node::Node(position, alliance, 16)
{

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
		for (vector<Node*>::iterator it = m_linked_nodes.begin(); it != m_linked_nodes.end(); it++)
		{
			if ((*it)->m_radar_activated == true)
			{
				m_radar_activated = true;
				break;
			}
		}
	}

	if (m_radar_activated == true)
	{
		m_radar_frequency_clock += deltaTime.asSeconds();
		if (m_radar_frequency_clock > m_radar_frequency)
		{
			m_radar_frequency_clock -= m_radar_frequency;
			CreateRadarWave();
		}
	}
	else
	{
		m_radar_frequency_clock = 0;
	}

	//hovering & selection
	m_hovered = false;
	if (IsHoveredByMouse() == true)
	{
		m_hovered = true;
		(*CurrentGame).m_hovered_node = this;

		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick && m_alliance == PlayerAlliance)
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

Wave* Node::CreateRadarWave()
{
	Wave* wave = new Wave(getPosition(), m_alliance, getRadius(), m_radar_speed, m_radar_range / m_radar_speed, m_radar_coverage, m_radar_direction);
	wave->m_emitter_node = this;
	(*CurrentGame).AddCircleObject(wave, WaveType);
	return wave;
}

void Node::WaveReception(Wave* wave)
{
	wave->m_lifespan = 0;
	wave->m_bounced_node->m_visible = true;
}

AllianceType Node::GetOriginAlliance()
{
	return m_alliance;
}