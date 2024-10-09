#include "SFTextPop.h"

SFTextPop::SFTextPop(SFText* text, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* target, float offset_positionY)
{
	setFont(*text->getFont());
	setCharacterSize(text->getCharacterSize());
	setFillColor(text->getFillColor());
	setPosition(sf::Vector2f(text->getPosition().x, text->getPosition().y + offset_positionY));

	m_visible = text->m_visible;
	m_team = text->m_team;
	m_alliance = text->m_alliance;
	m_offset_positionY = offset_positionY;

	m_garbageMe = false;

	m_distance_not_faded = distance_not_faded;
	m_distance_faded = distance_faded;

	m_total_pop_time = total_pop_time;

	m_alpha = 255;

	if (target)
	{
		m_target = target;
	}
	else
	{
		m_target = NULL;
	}
}

void SFTextPop::Update(Time deltaTime)
{
	float total_pop_distance = m_distance_not_faded + m_distance_faded;
	float pop_time_not_faded = m_total_pop_time * (m_distance_not_faded / (m_distance_not_faded + m_distance_faded));
	float pop_time_faded = m_total_pop_time - pop_time_not_faded;

	if (m_target)
	{
		setPosition(sf::Vector2f(m_target->getPosition().x - getGlobalBounds().width * 0.5f, m_target->getPosition().y - m_target->m_size.y * 0.5f + m_offset_positionY - (total_pop_distance / m_total_pop_time * m_timer_clock.getElapsedTime().asSeconds())));
	}
	else
	{
		setPosition(sf::Vector2f(getPosition().x, getPosition().y - (total_pop_distance / m_total_pop_time * deltaTime.asSeconds())));
	}

	if (m_timer_clock.getElapsedTime().asSeconds() > pop_time_not_faded)
	{
		Uint8 decay_ = (Uint8)(ceil(deltaTime.asSeconds() * 255 / pop_time_faded));
		if (m_alpha - decay_ > 0)
		{
			m_alpha -= decay_;
		}
		else
		{
			m_alpha = 0;
			m_garbageMe = true;
			m_visible = false;
		}

		setFillColor(Color(getFillColor().r, getFillColor().g, getFillColor().b, m_alpha));
	}
}