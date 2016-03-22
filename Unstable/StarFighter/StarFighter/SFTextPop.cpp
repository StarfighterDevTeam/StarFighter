#include "SFTextPop.h"

SFTextPop::SFTextPop(SFText* text, float distance_not_faded, float distance_faded, float total_pop_time, GameObject* target, sf::Vector2f offset)
{
	setFont(*text->getFont());
	setCharacterSize(text->getCharacterSize());
	setColor(text->getColor());
	setString(text->getString());
	setPosition(sf::Vector2f(text->getPosition().x + offset.x, text->getPosition().y + offset.y));

	m_visible = text->m_visible;
	m_GarbageMe = false;

	m_distance_not_faded = distance_not_faded;
	m_distance_faded = distance_faded;
	m_total_pop_time = total_pop_time;
	m_time_not_faded = 0;

	m_alpha = 255;
	m_offset = offset;

	if (target)
	{
		m_target = target;
	}
	else
	{
		m_target = NULL;
	}
}


SFTextPop::SFTextPop(SFText* text, float time_not_faded, float total_pop_time, GameObject* target, sf::Vector2f offset) : SFTextPop(text, 0, 0, total_pop_time, target, offset)
{
	m_time_not_faded = time_not_faded;
}

SFTextPop::~SFTextPop()
{
	m_target = NULL;
}

void SFTextPop::update(Time deltaTime)
{
	float total_pop_distance = m_distance_not_faded + m_distance_faded;
	float pop_time_not_faded = m_time_not_faded > 0 ? m_time_not_faded : m_total_pop_time * (m_distance_not_faded / (m_distance_not_faded + m_distance_faded));
	float pop_time_faded = m_total_pop_time - pop_time_not_faded;

	if (m_target)
	{
		setPosition(sf::Vector2f(m_target->getPosition().x - getGlobalBounds().width / 2 + m_offset.x, m_target->getPosition().y - m_target->m_size.y / 2 + m_offset.y - (total_pop_distance / m_total_pop_time * m_timer_clock.getElapsedTime().asSeconds())));
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
			m_GarbageMe = true;
		}

		setColor(Color(getColor().r, getColor().g, getColor().b, m_alpha));
	}
}