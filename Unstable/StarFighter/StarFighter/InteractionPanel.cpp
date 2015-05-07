#include "InteractionPanel.h"

InteractionPanel::InteractionPanel()
{
	m_selected_index = 0;

	m_arrow = new Independant(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
		sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));

	sf::Color _darkblue = sf::Color::Color(6, 87, 94, 255);//dark blue-green
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//semi-transparent white
	sf::Color _grey = sf::Color::Color(80, 80, 80, 255);//semi-transparent dark grey
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow

	m_panel.setSize(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT));
	m_panel.setOrigin(sf::Vector2f(INTERACTION_PANEL_WIDTH / 2, INTERACTION_PANEL_HEIGHT / 2));
	m_panel.setFillColor(sf::Color(10, 10, 10, 230));//dark grey
	m_panel.setOutlineThickness(2);
	m_panel.setOutlineColor(_darkblue);//dark blue-green

	try
	{
		m_font = new sf::Font();
		if (!m_font->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}

		ostringstream ss_destination;
		ss_destination << "???Unknown???";
		m_textDestination.setFont(*m_font);
		m_textDestination.setCharacterSize(20);
		m_textDestination.setColor(_yellow);
		m_textDestination.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP);
		m_textDestination.setString(ss_destination.str());

		for (int i = 0; i < NB_HAZARD_LEVELS; i++)
		{
			ostringstream ss;
			ss << "\n\nHazard " << i + 1;

			m_text[i].setFont(*m_font);
			m_text[i].setCharacterSize(18);
			m_text[i].setColor(_white);
			m_text[i].setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i));
			m_text[i].setString(ss.str());
		}

		ostringstream ss_help;
		ss_help << "\n\n\nFire: launch selected Hazard level\nBrake: up\nHyperspeed: down";
		m_textHelp.setFont(*m_font);
		m_textHelp.setCharacterSize(18);
		m_textHelp.setColor(_yellow);
		m_textHelp.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS));
		m_textHelp.setString(ss_help.str());
	}

	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

void InteractionPanel::Update(int max_unlocked_hazard_level)
{
	sf::Vector2f size = Independant::getSize_for_Direction(m_direction, sf::Vector2f(INTERACTION_PANEL_WIDTH / 2, INTERACTION_PANEL_HEIGHT / 2));
	sf::Vector2f position = Independant::getPosition_for_Direction(m_direction, sf::Vector2f(SCENE_SIZE_X / 2, PORTAL_HEIGHT + size.y + INTERACTION_PANEL_OFFSET_Y));

	m_panel.setPosition(position.x, position.y);

	for (int i = 0; i < NB_HAZARD_LEVELS; i++)
	{
		m_text[i].setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i) - (INTERACTION_PANEL_HEIGHT / 2));
	}

	m_textDestination.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP - (INTERACTION_PANEL_HEIGHT / 2));
	
	m_textHelp.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS) - (INTERACTION_PANEL_HEIGHT / 2));

	m_arrow->setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * m_selected_index) + (m_textDestination.getCharacterSize() / 2) + (m_text[0].getCharacterSize() * (4+1) / 2) - (INTERACTION_PANEL_HEIGHT / 2));

	ostringstream ss;
	ostringstream ss_greyed;

	for (int i = 0; i < NB_HAZARD_LEVELS; i++)
	{
		if (i > max_unlocked_hazard_level)
		{
			m_text[i].setColor(sf::Color(80, 80, 80, 255));//greyed
		}
		else
		{
			m_text[i].setColor(sf::Color(255, 255, 255, 255));//white
		}
	}
}

void InteractionPanel::Draw(sf::RenderTexture& screen)
{
	screen.draw(m_panel);
	screen.draw(*m_arrow);
	for (int i = 0; i < NB_HAZARD_LEVELS; i++)
	{
		screen.draw(m_text[i]);
	}
	screen.draw(m_textHelp);
	screen.draw(m_textDestination);
}