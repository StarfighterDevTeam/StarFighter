#include "HelpInterface.h"

extern Game* CurrentGame;

HelpInterface::HelpInterface()
{
	m_panel = NULL;
}

HelpInterface::~HelpInterface()
{
	Destroy();
}

void HelpInterface::Init(string& text_title, string& text_body, sf::Vector2f upper_left_anchor_position)
{
	//title
	if (text_title.empty() == false)
	{
		m_text_title.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text_title.setCharacterSize(18);
		m_text_title.setStyle(sf::Text::Bold);
		m_text_title.setColor((*CurrentGame).m_dico_colors[Color_Green_System]);
		m_text_title.setString(text_title);
		m_text_title.setPosition(sf::Vector2f(upper_left_anchor_position.x + 10, upper_left_anchor_position.y + 10));
	}

	//body
	if (text_body.empty() == false)
	{
		m_text_body.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text_body.setCharacterSize(16);
		m_text_body.setStyle(sf::Text::Bold);
		m_text_body.setColor(sf::Color::White);
		m_text_body.setString(text_body);
		m_text_body.setPosition(sf::Vector2f(upper_left_anchor_position.x + 10, upper_left_anchor_position.y + 20 + m_text_title.getGlobalBounds().height));
	}

	//background panel
	m_panel = new GameEntity(UI_None);
	float f1 = m_text_title.getGlobalBounds().height + 20 + ((int)(text_body.empty() == false) * (m_text_body.getGlobalBounds().height + 10));
	float f2 = m_text_title.getGlobalBounds().height + 20;
	m_panel->m_shape_container.setSize(sf::Vector2f(Maxf(m_text_title.getGlobalBounds().width, m_text_body.getGlobalBounds().width) + 20, m_text_title.getGlobalBounds().height + 26 + ((int)(text_body.empty() == false) * (m_text_body.getGlobalBounds().height + 10))));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(m_panel->m_shape_container.getSize().x * 0.5, m_panel->m_shape_container.getSize().y * 0.5));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(upper_left_anchor_position.x + m_panel->m_shape_container.getSize().x * 0.5, upper_left_anchor_position.y + m_panel->m_shape_container.getSize().y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
}

void HelpInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	m_text_title.setString("");
	m_text_body.setString("");
}

void HelpInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_text_title);
	screen.draw(m_text_body);
}