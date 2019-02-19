#include "PrisonersChoiceInterface.h"

extern Game* CurrentGame;

PrisonersChoiceInterface::PrisonersChoiceInterface()
{
	m_panel = NULL;
}

PrisonersChoiceInterface::~PrisonersChoiceInterface()
{
	
}

void PrisonersChoiceInterface::Init(Ship* enemy_ship)
{
	//get prisoners among survivors
	for (vector<CrewMember*>::iterator it = enemy_ship->m_crew.begin(); it != enemy_ship->m_crew.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			m_crew.push_back(*it);
		}
	}

	//no prisoner = no need for a menu
	int crew_size = m_crew.size();
	if (crew_size == 0)
	{
		return;
	}

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(PRISONERSCHOICEINTERFACE_SIZE_X, PRISONERSCHOICEINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f, PRISONERSCHOICEINTERFACE_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color(0, 0, 0, 255));
	m_panel->m_shape_container.setOutlineThickness(2.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f, REF_WINDOW_RESOLUTION_Y * 0.5f));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//panel text
	ostringstream ss_prisoners;
	ss_prisoners << "You captured " << crew_size << " prisoners. What will you do with them?";
	float offset_y = m_panel->m_position.y - PRISONERSCHOICEINTERFACE_SIZE_Y * 0.5f;
	offset_y += 30.f;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_prisoners.str());
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - m_narrative_text.getGlobalBounds().width * 0.5f, offset_y));

	//prisoners
	offset_y +=	100.f;
	int prisoners_per_line = 10;
	for (int i = 0; i < crew_size; i++)
	{
		float pos_x = m_panel->m_position.x - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f + 230 + ((i % prisoners_per_line) * (CREWMEMBER_SIZE + 10));
		float pos_y = offset_y + ((i / prisoners_per_line) * (CREWMEMBER_SIZE + 10));
		m_crew[i]->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
	}

	//prisoner card (focus)
	m_crew_focused = m_crew.front();
	m_crew_interface.Init(m_crew_focused);

	m_crew_focused->m_shape_container.setOutlineThickness(2.f);
	m_crew_focused->m_shape_container.setOutlineColor(sf::Color::Green);

	float pos_x = m_panel->m_position.x - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f + m_crew_interface.m_panel->m_shape_container.getSize().x * 0.5f + 20;
	m_crew_interface.SetPosition(sf::Vector2f(pos_x, offset_y + m_crew_interface.m_panel->m_shape_container.getSize().y * 0.5f - CREWMEMBER_SIZE * 0.5f));

	//life bar update
	int health = m_crew_focused->m_health;
	Bound(health, sf::Vector2i(0, m_crew_focused->m_health_max));

	float life_ratio = 1.0f * health / m_crew_focused->m_health_max;

	m_crew_interface.m_lifebar->m_shape.setSize(sf::Vector2f(life_ratio * CREWINTERFACE_LIFEBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));

	float threshold[3] = { 0.7, 0.5, 0.3 };
	if (life_ratio >= threshold[1])
	{
		m_crew_interface.m_lifebar->m_shape.setFillColor(sf::Color::Green);
	}
	else if (life_ratio >= threshold[2])
	{
		m_crew_interface.m_lifebar->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
	}
	else
	{
		m_crew_interface.m_lifebar->m_shape.setFillColor(sf::Color::Red);
	}

	ostringstream ss_life;
	ss_life << m_crew_focused->m_health << "/" << m_crew_focused->m_health_max;
	m_crew_interface.m_lifebar->m_text.setString(ss_life.str());
}

void PrisonersChoiceInterface::Update(sf::Time deltaTime)
{
	
}

void PrisonersChoiceInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_narrative_text);

	int crew_size = m_crew.size();
	for (int i = 0; i < crew_size; i++)
	{
		m_crew[i]->Draw(screen);
	}

	m_crew_interface.Draw(screen);
}