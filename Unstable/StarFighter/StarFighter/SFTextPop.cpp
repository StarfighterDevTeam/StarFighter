#include "Game.h"

extern Game* CurrentGame;

SFTextPop::SFTextPop()
{

}

SFTextPop::SFTextPop(SFText* text, float time_fade_in, float time_not_faded, float time_fade_out, GameObject* target, float speed_y, sf::Vector2f offset)
{
	setFont(*text->getFont());
	setCharacterSize(text->getCharacterSize());
	m_alpha = time_fade_in > 0 ? 0 : 255;
	setColor(sf::Color(text->getColor().r, text->getColor().g, text->getColor().b, m_alpha));
	setString(text->getString());
	if (target)
	{
		setPosition(sf::Vector2f(target->getPosition().x + offset.x, target->getPosition().y + offset.y));
	}
	else
	{
		setPosition(sf::Vector2f(text->getPosition().x + offset.x, text->getPosition().y + offset.y));
	}
	
	m_visible = text->m_visible;
	m_garbageMe = false;
	m_DontGarbageMe = false;

	m_time_fade_in = time_fade_in;
	m_time_not_faded = time_not_faded;
	m_time_fade_out = time_fade_out;
	m_target = target;
	m_speed_y = speed_y;
	m_offset = offset;
	m_time = sf::seconds(0);
}

void SFTextPop::update(Time deltaTime, float hyperspeedMultiplier)
{
	if (hyperspeedMultiplier < 1.0f)
	{
		m_time += deltaTime * hyperspeedMultiplier;
	}
	else
	{
		m_time += deltaTime;
	}

	//fade in
	if (m_time.asSeconds() < m_time_fade_in)
	{
		Uint8 alpha_change = (Uint8)(ceil(deltaTime.asSeconds() * hyperspeedMultiplier * 255 / m_time_fade_in));
		if (m_alpha + alpha_change < 255)
		{
			m_alpha += alpha_change;
		}
		else
		{
			m_alpha = 255;
		}
		//apply new alpha value
		setColor(Color(getColor().r, getColor().g, getColor().b, m_alpha));
	}
	//fade out
	else if (m_time_not_faded >= 0 && m_time.asSeconds() > m_time_fade_in + m_time_not_faded)
	{
		Uint8 alpha_change = (Uint8)(ceil(deltaTime.asSeconds() * hyperspeedMultiplier * 255 / m_time_fade_out));
		if (m_alpha - alpha_change > 0)
		{
			m_alpha -= alpha_change;
		}
		else
		{
			m_alpha = 0;
			m_garbageMe = true;
			m_visible = false;
		}
		//apply new alpha value
		setColor(Color(getColor().r, getColor().g, getColor().b, m_alpha));
	}

	//move
	float newspeed = m_speed_y;

	float l_hyperspeedMultiplier = hyperspeedMultiplier < 1 ? hyperspeedMultiplier : 1;

	//slowmotion
	newspeed += (l_hyperspeedMultiplier - 1) * (*CurrentGame).m_vspeed;

	if (m_target)
	{
		setPosition(sf::Vector2f(m_target->getPosition().x - getGlobalBounds().width / 2 + m_offset.x, m_target->getPosition().y - m_target->m_size.y / 2 + m_offset.y - newspeed * deltaTime.asSeconds()));
	}
	else
	{
		setPosition(sf::Vector2f(getPosition().x, getPosition().y - newspeed * deltaTime.asSeconds()));
	}
}