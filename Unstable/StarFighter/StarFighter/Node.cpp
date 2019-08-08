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
	m_radar_direction = alliance == PlayerAlliance ? 0 : 180;
	m_radar_coverage = 60;

	m_is_terminal_node = false;
	m_ghost = false;

	m_visible = true;// alliance == PlayerAlliance;
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

AllianceType Node::GetOriginAlliance()
{
	return m_alliance;
}

bool Node::IsColliding(Wave* wave, float direction)
{
	if (IsInsideAngleCoverage(direction, wave->m_angle_coverage, wave->m_angle_direction) == false)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < CIRCLE_POINTS_COUNT; i++)
		{
			float ang = i * 360.f / (CIRCLE_POINTS_COUNT - 1);
			float delta_angle = atan(getRadius() / (getRadius() + wave->getRadius())) * 180.f / M_PI;

			if (IsInsideAngleCoverage(ang, delta_angle * 2, direction) == true && wave->m_points[i * 2].color != sf::Color(0, 0, 0, 0))
			{
				return true;
			}
		}

		return false;
	}
}

Wave* Node::CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave)
{
	Wave* new_wave = new Wave(position, NeutralAlliance, radius, wave->m_expansion_speed, wave->m_lifespan, wave->m_angle_coverage, direction + 180);
	new_wave->m_bounced_node = this;
	new_wave->m_emitter_node = wave->m_emitter_node;
	(*CurrentGame).AddCircleObject(new_wave, WaveType);
	wave->m_bounced_nodes.push_back(this);

	//masking wave sector of incidence
	float delta_angle = atan(getRadius() / (getRadius() + wave->getRadius())) * 180.f / M_PI;
	for (int i = 0; i < CIRCLE_POINTS_COUNT; i++)
	{
		float ang = i * 360.f / (CIRCLE_POINTS_COUNT - 1);
		if (IsInsideAngleCoverage(ang, delta_angle * 2, direction))
		{
			wave->m_points[i * 2].color = sf::Color(0, 0, 0, 0);
			wave->m_points[i * 2 + 1].color = sf::Color(0, 0, 0, 0);
		}
	}

	//RWR (radar warning receiver)
	if (wave->m_alliance != NeutralAlliance && wave->m_alliance != m_alliance)
	{
		wave->m_emitter_node->m_visible = true;
		for (vector<CircleObject*>::iterator it = (*CurrentGame).m_sceneCircleObjects[wave->m_alliance][WaveType].begin(); it != (*CurrentGame).m_sceneCircleObjects[wave->m_alliance][WaveType].end(); it++)
		{
			Wave* wave_iterator = (Wave*)(*it);
			if (wave_iterator->m_emitter_node == wave->m_emitter_node)
			{
				wave_iterator->m_visible = true;
			}
		}
	}

	return new_wave;
}

void Node::WaveReception(Wave* wave)
{
	wave->m_bounced_node->m_visible = true;

	for (vector<CircleObject*>::iterator it = (*CurrentGame).m_sceneCircleObjects[NeutralAlliance][WaveType].begin(); it != (*CurrentGame).m_sceneCircleObjects[NeutralAlliance][WaveType].end(); it++)
	{
		Wave* wave_iterator = (Wave*)(*it);
		if (wave_iterator->m_bounced_node == wave->m_bounced_node)// && wave_iterator->m_emitter_node->m_alliance == PlayerAlliance)
		{
			wave_iterator->m_visible = true;
		}
	}

	wave->m_lifespan = 0;
	//masking wave sector of incidence
	/*
	float delta_angle = atan(getRadius() / (getRadius() + wave->getRadius())) * 180.f / M_PI;
	for (int i = 0; i < CIRCLE_POINTS_COUNT; i++)
	{
		float ang = i * 360.f / (CIRCLE_POINTS_COUNT - 1);
		if (IsInsideAngleCoverage(ang, delta_angle * 2, wave->m_angle_direction))
		{
			wave->m_points[i * 2].color = sf::Color(0, 0, 0, 0);
			wave->m_points[i * 2 + 1].color = sf::Color(0, 0, 0, 0);
		}
	}
	*/
}