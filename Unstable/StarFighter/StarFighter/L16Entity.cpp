#include "L16Entity.h"

extern Game* CurrentGame;

using namespace sf;

L16Entity::L16Entity(sf::Vector2f position, AllianceType alliance, float radius) : CircleObject(alliance)
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
	m_radar_frequency_clock = 0;
	m_radar_bounce_feedback_clock = 0;

	m_visible = true;// alliance == PlayerAlliance;
	m_mission = L16Mission_Idle;
}

L16Entity::~L16Entity()
{

}

void L16Entity::update(sf::Time deltaTime)
{
	//active radar
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
		//setOutlineColor(sf::Color(0, 255, 0, 255));
	}
	if (m_hovered == true)
	{
		setOutlineColor(sf::Color(255, 255, 255, 255));
	}

	//radar bounced feedback
	if (m_radar_bounce_feedback_clock > 0 && m_alliance == PlayerAlliance)
	{
		setOutlineColor(sf::Color(255, 0, 0, 255));
		m_radar_bounce_feedback_clock -= deltaTime.asSeconds();
	}

	CircleObject::update(deltaTime);
}

bool L16Entity::IsHoveredByMouse()
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

void L16Entity::ResetColor()
{
	setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, GHOST_ALPHA_VALUE));
	setOutlineColor(sf::Color(m_color.r, m_color.g, m_color.b, 255));
}

AllianceType L16Entity::GetOriginAlliance()
{
	return m_alliance;
}

bool L16Entity::IsColliding(Wave* wave, float direction)
{
	if (IsInsideAngleCoverage(direction, wave->m_angle_coverage, wave->m_angle_direction) == false)
	{
		return false;
	}
	else
	{
		if (wave->m_bounced_entities.empty() == true)
		{
			return true;
		}
		else
		{
			float delta_angle = atan(getRadius() / (getRadius() + wave->getRadius())) * 180.f / M_PI;
			for (int i = 0; i < CIRCLE_POINTS_COUNT; i++)
			{
				float ang = i * 360.f / (CIRCLE_POINTS_COUNT - 1);
				if (IsInsideAngleCoverage(ang, delta_angle * 2, direction) == true && wave->m_points[i * 2].color != sf::Color(0, 0, 0, 0))
				{
					return true;
				}
			}

			return false;
		}
	}
}

Wave* L16Entity::CreateRadarWave()
{
	Wave* wave = new Wave(getPosition(), m_alliance, getRadius(), m_radar_wavespeed, m_radar_range / m_radar_wavespeed, m_radar_coverage, m_radar_heading);
	wave->m_emitter_entity = this;
	(*CurrentGame).AddCircleObject(wave, Circle_Wave);
	return wave;
}

Wave* L16Entity::CreateWaveBounce(sf::Vector2f position, float radius, float direction, Wave* wave)
{
	float delta_angle = atan(getRadius() / (getRadius() + wave->getRadius())) * 180.f / M_PI;

	Wave* new_wave = new Wave(position, NeutralAlliance, radius, wave->m_expansion_speed, wave->m_lifespan, MinBetweenValues(60, wave->m_angle_coverage), direction + 180);
	new_wave->m_bounced_entity = this;
	new_wave->m_emitter_entity = wave->m_emitter_entity;
	(*CurrentGame).AddCircleObject(new_wave, Circle_Wave);
	wave->m_bounced_entities.push_back(this);

	//masking wave sector of incidence
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
		wave->m_emitter_entity->RevealEntity();

		for (vector<CircleObject*>::iterator it = (*CurrentGame).m_sceneCircleObjects[wave->m_alliance][Circle_Wave].begin(); it != (*CurrentGame).m_sceneCircleObjects[wave->m_alliance][Circle_Wave].end(); it++)
		{
			Wave* wave_iterator = (Wave*)(*it);
			if (wave_iterator->m_emitter_entity == wave->m_emitter_entity)
			{
				wave_iterator->m_visible = true;
			}
		}
	}

	return new_wave;
}

void L16Entity::WaveReception(Wave* wave)
{
	wave->m_bounced_entity->RevealEntity();

	for (vector<CircleObject*>::iterator it = (*CurrentGame).m_sceneCircleObjects[NeutralAlliance][Circle_Wave].begin(); it != (*CurrentGame).m_sceneCircleObjects[NeutralAlliance][Circle_Wave].end(); it++)
	{
		Wave* wave_iterator = (Wave*)(*it);
		if (wave_iterator->m_bounced_entity == wave->m_bounced_entity)// && wave_iterator->m_emitter_entity->m_alliance == PlayerAlliance)
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

void L16Entity::RevealEntity()
{
	m_visible = true;
	m_radar_bounce_feedback_clock = 1;
}