#include "SFDialogPanel.h"

extern Game* CurrentGame;

SFDialogPanel::SFDialogPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Dialog)
{
	m_playerShip = playerShip;
	m_dialog = playerShip->m_targetDialogs.front();

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	setOutlineThickness(2);

	//panel position and color
	sf::Vector2f position = sf::Vector2f(DIALOG_PANEL_OFFSET_X + size.x / 2, SCENE_SIZE_Y - DIALOG_PANEL_OFFSET_Y - size.y / 2);
	if (m_dialog->m_enemy_speaking)
	{
		setPosition(GameObject::getPosition_for_Direction(DIRECTION_DOWN, position));
		setFillColor(sf::Color(255, 27, 27, 50));
		setOutlineColor(sf::Color(255, 0, 0, 255));
		m_title_text.setColor(sf::Color(255, 0, 0, 255));
	}
	else
	{
		setPosition(position);
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
	
}

void SFDialogPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);
		screen.draw(m_picture);
	}
}

Dialog* SFDialogPanel::GetDialog()
{
	return m_dialog;
}