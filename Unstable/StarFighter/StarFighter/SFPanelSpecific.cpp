#include "SFPanelSpecific.h"

extern Game* CurrentGame;

SFPanelSpecific::SFPanelSpecific(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts strings
	ostringstream ss_title;
	ss_title << "Title";
	m_title_text.setString(ss_title.str());

	//options texts
	m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	m_actions->SetString("Enter", ActionButton_A);
	m_actions->SetString("Hazard down", ActionButton_Y);
	m_actions->SetString("Hazard up", ActionButton_X);

	m_actions_with_selection = new SFActionBoxWithSelection();
	for (int i = 0; i < 4; i++)
	{
		stringstream ss;
		ss << "Action " << i;
		m_actions_with_selection->AddOption(ss.str(), (*CurrentGame).m_font[Font_Arial]);
	}

	//size and position of panel
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	sf::Vector2f position = sf::Vector2f(540, 540);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//options texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
}

SFPanelSpecific::~SFPanelSpecific()
{
	
}

void SFPanelSpecific::Update(const float DTIME)
{
	
}

void SFPanelSpecific::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);

		if (m_actions)
		{
			m_actions->Draw(screen);
		}
	}
}