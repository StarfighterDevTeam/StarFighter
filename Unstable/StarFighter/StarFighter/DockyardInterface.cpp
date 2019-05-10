#include "DockyardInterface.h"

extern Game* CurrentGame;

DockyardInterface::DockyardInterface()
{
	m_panel = NULL;
	m_leave_button = NULL;
}

DockyardInterface::~DockyardInterface()
{
	//delete m_panel;
	//delete m_drowning_bar;

	Destroy();
}

void DockyardInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	delete m_leave_button;
	m_leave_button = NULL;
}

void DockyardInterface::Init(Ship* ship, Location* location)
{
	m_ship = ship;
	m_location = location;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(DOCKYARDINTERFACE_SIZE_X, DOCKYARDINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(DOCKYARDINTERFACE_SIZE_X * 0.5, DOCKYARDINTERFACE_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - CREWUNBOARDINTERFACE_SIZE_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//narrative text
	string ss_narrative = "Welcome to your dockyard.\nPlease suit yourself and buy all what you need for your ship.";
	
	float offset_y = m_panel->m_position.y - DOCKYARDINTERFACE_SIZE_Y * 0.5;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_narrative);
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5 + 20, offset_y));

	//leave button
	offset_y += m_narrative_text.getGlobalBounds().height + m_narrative_text.getCharacterSize() + 30;
	m_leave_button = new GameEntity(UI_None);
	m_leave_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_leave_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_leave_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_leave_button->m_shape_container.setOutlineThickness(2);
	m_leave_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_leave_button->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x, offset_y));

	m_leave_button->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_leave_button->m_text.setCharacterSize(16);
	m_leave_button->m_text.setStyle(sf::Text::Bold);
	m_leave_button->m_text.setColor(sf::Color::White);
	m_leave_button->m_text.setString("Leave");
	m_leave_button->m_text.setPosition(sf::Vector2f(m_leave_button->m_shape_container.getPosition().x - m_leave_button->m_text.getGlobalBounds().width * 0.5, offset_y - m_leave_button->m_text.getCharacterSize() * 0.65));
}

void DockyardInterface::Update(sf::Time deltaTime)
{
	//leave button's hovering feedback
	m_leave_button->ButtonUpdate();
}

void DockyardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_narrative_text);

	m_leave_button->Draw(screen);
}