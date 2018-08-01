#include "SFPanelSpecific.h"

extern Game* CurrentGame;

SFPanelSpecific::SFPanelSpecific(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setColor(sf::Color(255, 255, 255, 255));

	//texts strings
	ostringstream ss;
	ss << "Speed: ";
	m_text.setString(ss.str());

	//size and position of panel
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	sf::Vector2f position = sf::Vector2f(116, 32);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += INTERACTION_INTERBLOCK + m_text.getCharacterSize();

	m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
}

SFPanelSpecific::~SFPanelSpecific()
{

}

void SFPanelSpecific::Update(sf::Time deltaTime)
{
	//texts strings
	ostringstream ss;
	ss << "Speed: " << m_playerShip->m_speed.x << " | " << m_playerShip->m_speed.y;
	ostringstream ss_state;
	switch (m_playerShip->m_state)
	{
		case Character_Ground:
		{
			ss_state << "Ground (0)";
			break;
		}
		case Character_Jump1:
		{
								ss_state << "Jump_1 (1): speed z " << m_playerShip->m_speed_z;// << " / " << m_playerShip->m_jump_target_altitude;
			break;
		}
		case Character_Fall1:
		{
			ss_state << "Fall_1 (3): " << m_playerShip->m_speed.y;
			break;
		}
	}
	ss << "\n" << ss_state.str();
	ss << "\nAltitude: " << m_playerShip->m_altitude;//getPosition().y - 1080 / 2;
	m_text.setString(ss.str());

	m_text.setPosition(sf::Vector2f(m_playerShip->getPosition().x - m_text.getLocalBounds().width / 2, m_playerShip->getPosition().y + 100));
}

void SFPanelSpecific::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		//SFPanel::Draw(screen);
		screen.draw(m_text);
	}
}