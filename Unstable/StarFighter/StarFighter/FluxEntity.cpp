#include "FluxEntity.h"

extern Game* CurrentGame;

FluxEntity::FluxEntity()
{
	FluxInitialization();
}

FluxEntity::FluxEntity(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	FluxInitialization();
}

void FluxEntity::FluxInitialization()
{
	m_isAutogeneratingFlux = false;
	m_wasting_flux = false;
	m_flux_waste = 0;
	m_flux_waste_delay = FLUXOR_WASTE_DELAY;
	m_transfer_buffer = 0;
	m_transfert_buffer_memory = 0;

	//hud
	m_displaying_flux = true;
	m_flux_text = NULL;
	m_flux_gauge = NULL;

	//update grid index
	m_curGridIndex = (*CurrentGame).GetGridIndex(getPosition());

	//warning feedback
	if (!(*CurrentGame).m_player_warnings)
	{
		(*CurrentGame).m_player_warnings = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "Assets/2D/warning.png", sf::Vector2f(42, 38), sf::Vector2f(21, 19), 4, 3);
	}
	m_warning = (*CurrentGame).m_player_warnings->Clone();
	m_warning->setPosition(getPosition());
	m_warning->m_visible = false;
	m_warning->m_team = m_team;
	m_warning->m_alliance = m_alliance;
	(*CurrentGame).addToScene(m_warning, WarningLayer, BackgroundObject);
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

	if (m_warning)
	{
		m_warning->m_visible = false;
		m_warning->m_GarbageMe = true;
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

				if (m_flux_max > 0)
				{
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

void FluxEntity::UpdateWarningFeedback()
{
	//setting the correct animation line if not already set
	if (m_warning && m_currentAnimationIndex != m_team)
	{
		m_warning->setAnimationLine(m_team);
	}

	//warning feedback (in case of attack)
	if (m_warning && m_warning_feedback_activated)
	{
		m_warning->m_visible = true;
		m_warning_feedback_activated = false;
		m_warning_clock.restart();
	}
	if (m_warning && m_warning->m_visible)
	{
		m_warning->setPosition(sf::Vector2f(getPosition().x, getPosition().y - m_size.y / 2 - WARNING_OFFSET_Y));
		if (m_warning_clock.getElapsedTime().asSeconds() > WARNING_FEEDBACK_DURATION)
		{
			m_warning->m_visible = false;
		}
	}
}

GameObject* FluxEntity::CreateWarningFeedback(sf::Vector2f position)
{
	GameObject* warning = (*CurrentGame).m_player_warnings->Clone();
	warning->setPosition(position);
	warning->m_team = m_team;
	warning->m_alliance = m_alliance;
	warning->setAnimationLine(m_team);
	warning->m_life_time = WARNING_FEEDBACK_DURATION;
	(*CurrentGame).addToScene(warning, WarningLayer, BackgroundObject);

	return warning;
}