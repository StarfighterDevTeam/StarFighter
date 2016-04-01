#include "SFDialogPanel.h"

extern Game* CurrentGame;

SFDialogPanel::SFDialogPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Dialog)
{
	m_playerShip = playerShip;
 	m_dialog = playerShip->m_targetDialogs.front();

	if (m_dialog->m_duration == 0)
	{
		(*CurrentGame).m_waiting_for_dialog_validation = true;
	}

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	setOutlineThickness(2);

	//is dialog displayed up or down
	m_is_downscreen = (*CurrentGame).m_direction == DIRECTION_DOWN ? m_dialog->m_enemy_speaking : !m_dialog->m_enemy_speaking;

	//panel position and color
	setPosition(sf::Vector2f(DIALOG_PANEL_OFFSET_X + size.x / 2, SCENE_SIZE_Y - DIALOG_PANEL_OFFSET_Y - size.y / 2));
	m_target_position = sf::Vector2f(-size.x / 2, SCENE_SIZE_Y - DIALOG_PANEL_OFFSET_Y - size.y / 2);
	if (!m_is_downscreen)
	{
		setPosition(GameObject::getPosition_for_Direction(DIRECTION_DOWN, getPosition()));
		m_target_position = GameObject::getPosition_for_Direction(DIRECTION_DOWN, m_target_position);
	}
	
	if (m_dialog->m_enemy_speaking)
	{
		setFillColor(sf::Color(255, 27, 27, 50));
		setOutlineColor(sf::Color(255, 0, 0, 255));
		m_title_text.setColor(sf::Color(255, 0, 0, 255));
	}
	else
	{
		setFillColor(sf::Color(27, 27, 255, 50));
		setOutlineColor(sf::Color(0, 0, 255, 255));
		m_title_text.setColor(sf::Color(0, 0, 255, 255));
	}

	//texts
	m_title_text.setString(m_dialog->m_title);
	m_text.setString(m_dialog->m_body);

	//content position
	m_picture = GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), m_dialog->m_picture_name, sf::Vector2f(DIALOG_PANEL_PORTRAIT_SIZE_X, DIALOG_PANEL_PORTRAIT_SIZE_Y));
	m_picture.setPosition(getPosition().x - getSize().x / 2 + m_picture.m_size.x / 2, getPosition().y);

	float text_height = INTERACTION_PANEL_MARGIN_TOP + m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + m_picture.m_size.x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE + m_text.getGlobalBounds().height / 2;
	m_text.setPosition(getPosition().x - getSize().x / 2 + m_picture.m_size.x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//buttons
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_actions_text.setCharacterSize(18);
	m_actions_text.setColor(Color::White);
	ostringstream ss;
	if (m_dialog->m_next_dialog_name.empty() || m_dialog->m_next_dialog_name.compare("0") == 0)
	{
		ss << "Continue";
	}
	else
	{
		ss << "Next";
	}
	m_actions_text.setString(ss.str());
	m_buttons = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_BUTTON_A_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT),
		sf::Vector2f(INTERACTION_BUTTON_WIDTH / 2, INTERACTION_BUTTON_HEIGHT / 2));
	m_actions_text.setPosition(getPosition().x - getSize().x / 2 + m_picture.m_size.x + INTERACTION_INTERBLOCK +INTERACTION_BUTTON_MARGIN + m_buttons.m_size.x, getPosition().y + getSize().y / 2 - INTERACTION_INTERBLOCK - m_actions_text.getCharacterSize());
	m_buttons.setPosition(m_actions_text.getPosition().x - m_buttons.m_size.x, m_actions_text.getPosition().y + m_actions_text.getCharacterSize() - 6);
}

SFDialogPanel::~SFDialogPanel()
{
	if (m_dialog)
	{
		delete m_dialog;
		m_dialog = NULL;
	}
}

void SFDialogPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//fade in
	if (m_dialog && m_dialog->m_fade_in)
	{
		if (m_is_downscreen && getPosition().x < m_target_position.x)
		{
			setPosition(getPosition().x + 1, getPosition().y);
		}
		else if (!m_is_downscreen && getPosition().x > m_target_position.x)
		{
			setPosition(getPosition().x - 1, getPosition().y);
		}
	}
}

void SFDialogPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);
		screen.draw(m_picture);

		if (m_dialog->m_duration == 0)
		{
			screen.draw(m_actions_text);
			screen.draw(m_buttons);
		}
	}
}

Dialog* SFDialogPanel::GetDialog()
{
	return m_dialog;
}

float SFDialogPanel::GetDuration()
{
	return m_dialog->m_duration;
}

float SFDialogPanel::GetDurationClockElpased()
{
	return m_duration_clock.getElapsedTime().asSeconds();
}