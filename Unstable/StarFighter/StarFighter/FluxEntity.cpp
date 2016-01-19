#include "FluxEntity.h"

extern Game* CurrentGame;

FluxEntity::FluxEntity()
{
	m_flux_text = NULL;
	m_flux_gauge = NULL;
}

FluxEntity::FluxEntity(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_flux = 0;
	m_flux_max = 1;
	m_isAutogeneratingFlux = false;
	m_wasting_flux = false;
	m_flux_waste = 0;;
	m_flux_waste_delay = FLUXOR_WASTE_DELAY;

	m_displaying_flux = false;

	m_flux_text = NULL;
	m_flux_gauge = NULL;

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());
}

FluxEntity::~FluxEntity()
{
	if (m_flux_text)
	{
		m_flux_text->m_visible = false;
		m_flux_text->m_GarbageMe = true;
	}
	if (m_flux_gauge)
	{
		m_flux_gauge->m_visible = false;
		m_flux_gauge->m_GarbageMe = true;
	}
}

void FluxEntity::WastingFlux()
{
	if (m_wasting_flux && m_flux > 0)
	{
		if (m_flux_waste_clock.getElapsedTime().asSeconds() > m_flux_waste_delay)
		{
			if (m_flux_waste <= m_flux)
			{
				m_flux -= m_flux_waste;
			}
			else
			{
				m_flux = 0;
			}

			m_flux_waste_clock.restart();

			//feedback
			if (USE_FEEDBACK_WASTING)
			{
				SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 16, sf::Color::Cyan, getPosition(), m_team);
				text_feedback->m_alliance = m_alliance;
				text_feedback->setString("-1");
				SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
				pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
				delete text_feedback;
				(*CurrentGame).addToFeedbacks(pop_feedback);
			}
		}
	}
}

void FluxEntity::AddFluxGauge(GaugeStyles gauge, sf::Vector2f offset)
{
	m_flux_gauge = (*CurrentGame).m_flux_gauges[0]->Clone();
	m_flux_gauge->m_offset = offset;
}

void FluxEntity::SetTeam(PlayerTeams team, TeamAlliances alliance)
{
	m_team = team;
	m_alliance = alliance;
	if (m_flux_text)
	{
		m_flux_text->m_team = team;
		m_flux_text->m_alliance = alliance;
	}
}

bool FluxEntity::AutogenerateFlux()
{
	if (m_isAutogeneratingFlux)
	{
		if (m_flux < m_flux_max || m_flux_max == 0)
		{
			if (m_flux_autogeneration_clock.getElapsedTime().asSeconds() > m_flux_autogeneration_time)
			{
				m_flux++;
				m_flux_autogeneration_clock.restart();

				//feedback
				if (USE_FEEDBACK_AUTOGENERATION)
				{
					SFText* text_feedback = new SFText((*CurrentGame).m_fonts[Font_Arial], 24, sf::Color::Cyan, getPosition(), m_team);
					text_feedback->m_alliance = m_alliance;
					text_feedback->setString("+1");
					SFTextPop* pop_feedback = new SFTextPop(text_feedback, TEXT_POP_DISTANCE_NOT_FADED, TEXT_POP_DISTANCE_FADE_OUT, TEXT_POP_TOTAL_TIME, NULL, sf::Vector2f(0, 0));
					pop_feedback->setPosition(sf::Vector2f(getPosition().x - pop_feedback->getGlobalBounds().width / 2, getPosition().y));
					delete text_feedback;
					(*CurrentGame).addToFeedbacks(pop_feedback);
				}

				return true;
			}

			return false;
		}
		else
		{
			m_flux_autogeneration_clock.restart();
		}
	}

	return false;
}
