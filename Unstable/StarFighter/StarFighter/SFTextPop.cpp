#include "SFTextPop.h"

extern Game* CurrentGame;

SFTextPop::SFTextPop()
{

}

SFTextPop::SFTextPop(sf::Text* text, float distance_not_faded, float distance_faded, float total_pop_time, GameEntity* target, float offset_positionY)
{
	//setFont(*text->getFont());
	//setCharacterSize(text->getCharacterSize());
	//setColor(text->getColor());
	//setPosition(sf::Vector2f(text->getPosition().x, text->getPosition().y + offset_positionY));
	//
	//m_offset_positionY = offset_positionY;
	//
	//m_distance_not_faded = distance_not_faded;
	//m_distance_faded = distance_faded;
	//
	//m_total_pop_time = total_pop_time;
	//
	//m_alpha = 255;
	//
	//if (target)
	//{
	//	m_target = target;
	//}
	//else
	//{
	//	m_target = NULL;
	//}
}

SFTextPop::~SFTextPop()
{

}

void SFTextPop::Update(Time deltaTime)
{
	float total_pop_distance = m_distance_not_faded + m_distance_faded;
	float pop_time_not_faded = m_total_pop_time * (m_distance_not_faded / (m_distance_not_faded + m_distance_faded));
	float pop_time_faded = m_total_pop_time - pop_time_not_faded;

	if (m_target != NULL)
	{
		setPosition(sf::Vector2f(m_target->getPosition().x - getGlobalBounds().width * 0.5f, m_target->m_position.y - (m_target->m_shape_container.getSize().y * 0.5f) - m_offset_positionY - (total_pop_distance / m_total_pop_time * m_timer_clock.getElapsedTime().asSeconds())));
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
		}

		setColor(Color(getColor().r, getColor().g, getColor().b, m_alpha));
	}
}



SFTextPop* SFTextPop::CreateSFTextPop(const Font& font, unsigned int size, Uint32 style, const Color& color, string text_string, float distance_not_faded, float distance_faded, float total_pop_time, GameEntity* target, float offset_positionY)
{
	SFTextPop* text = new SFTextPop();

	text->setFont(font);
	text->setCharacterSize(size);
	text->setStyle(style);
	text->setColor(color);
	text->setString(text_string);
	text->setPosition(sf::Vector2f(target->m_position.x - text->getGlobalBounds().width * 0.5f, target->m_position.y - (target->m_shape_container.getSize().y * 0.5f) - offset_positionY));

	text->m_offset_positionY = offset_positionY;

	text->m_distance_not_faded = distance_not_faded;
	text->m_distance_faded = distance_faded;

	text->m_total_pop_time = total_pop_time;

	text->m_alpha = 255;

	if (target != NULL)
	{
		text->m_target = target;
	}
	else
	{
		text->m_target = NULL;
	}

	(*CurrentGame).m_texts_pop.push_back(text);

	return text;
}