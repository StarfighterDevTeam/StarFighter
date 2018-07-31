#include "SFPanelSpecific.h"

extern Game* CurrentGame;

SFPanelSpecific::SFPanelSpecific(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setColor(sf::Color(255, 255, 255, 255));

	//setFillColor(sf::Color(0, 0, 0, 0));
	//setOutlineColor(sf::Color(0, 0, 0, 0));

	//texts strings
	ostringstream ss;
	ss << "Boids living:";
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
	ss << "Boids living: " << (*CurrentGame).m_boids_alive;
	ss << "\nPredator living: " << (*CurrentGame).m_predators_alive;
	m_text.setString(ss.str());
}

void SFPanelSpecific::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		//SFPanel::Draw(screen);
		//screen.draw(m_title_text);
		screen.draw(m_text);
	}
}