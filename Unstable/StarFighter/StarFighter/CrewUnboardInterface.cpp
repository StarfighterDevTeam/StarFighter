#include "CrewUnboardInterface.h"

extern Game* CurrentGame;

CrewUnboardInterface::CrewUnboardInterface()
{
	m_panel = NULL;
	m_hovered = NULL;
}

CrewUnboardInterface::~CrewUnboardInterface()
{
	//delete m_panel;
	//delete m_drowning_bar;

	Destroy();
}

void CrewUnboardInterface::Destroy()
{
	//m_crew_interface.Destroy();

	delete m_panel;

	m_panel = NULL;
	m_unboarded.clear();

	for (int i = 0; i < m_slots_avaible; i++)
	{
		delete m_crew_slots[i];
	}
	m_crew_slots.clear();
}

void CrewUnboardInterface::Init(Ship* ship)
{
	m_ship = ship;
	m_slots_avaible = ship->m_lifeboats;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CREWUNBOARDINTERFACE_SIZE_X, CREWUNBOARDINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CREWUNBOARDINTERFACE_SIZE_X * 0.5f, CREWUNBOARDINTERFACE_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - CREWUNBOARDINTERFACE_SIZE_X * 0.5f, REF_WINDOW_RESOLUTION_Y * 0.5f));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//narrative text
	ostringstream ss_survivors;
	ss_survivors << "Your lifeboats allow for " << m_slots_avaible << " crew members to unboard.\nRight click on crew members to select them for the mission.";
	float offset_y = m_panel->m_position.y - CREWUNBOARDINTERFACE_SIZE_Y * 0.5f;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_survivors.str());
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5f + 20, offset_y));

	offset_y += 90;
	ostringstream ss_slots;
	ss_slots << 0 << " / " << m_slots_avaible;
	m_crew_slot_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_crew_slot_text.setCharacterSize(18);
	m_crew_slot_text.setStyle(sf::Text::Bold);
	m_crew_slot_text.setColor(sf::Color::White);
	m_crew_slot_text.setString(ss_slots.str());
	m_crew_slot_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5f + 20, offset_y - m_crew_slot_text.getCharacterSize() * 0.5f));

	//unboarders
	offset_y += m_crew_slot_text.getCharacterSize() + 20;

	int crew_per_line = 10;
	float prisoners_offset_x = m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5f + 20;
	for (int i = 0; i < m_slots_avaible; i++)
	{
		float pos_x = prisoners_offset_x + (CREWMEMBER_SIZE * 0.5f) + ((i % crew_per_line) * (CREWMEMBER_SIZE + 10));
		float pos_y = offset_y + ((i / crew_per_line) * (CREWMEMBER_SIZE + 10));
		offset_y = Maxf(offset_y, pos_y);
	
		GameEntity* crew_slot = new GameEntity(UI_None);
		crew_slot->m_shape_container.setSize(sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE));
		crew_slot->m_shape_container.setOrigin(sf::Vector2f(CREWMEMBER_SIZE * 0.5f, CREWMEMBER_SIZE * 0.5f));
		crew_slot->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
		crew_slot->m_shape_container.setOutlineThickness(2.f);
		crew_slot->m_shape_container.setOutlineColor(sf::Color::Black);
		crew_slot->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
		crew_slot->m_position = crew_slot->m_shape_container.getPosition();
		m_crew_slots.push_back(crew_slot);
	}
}

void CrewUnboardInterface::Update(sf::Time deltaTime, CrewMember* unboarded_crew)
{
	if (unboarded_crew != NULL)
	{
		m_unboarded.push_back(unboarded_crew);
		unboarded_crew->m_position = m_crew_slots[m_unboarded.size() - 1]->m_position;
		unboarded_crew->Update(deltaTime);//update position + reset hovering
	}

	ostringstream ss_slots;
	ss_slots << m_unboarded.size() << " / " << m_slots_avaible;
	m_crew_slot_text.setString(ss_slots.str());
}

void CrewUnboardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	screen.draw(m_crew_slot_text);
	screen.draw(m_narrative_text);

	int crew_size = m_crew_slots.size();
	for (int i = 0; i < crew_size; i++)
	{
		m_crew_slots[i]->Draw(screen);
	}

	int unboarded_size = m_unboarded.size();
	for (int i = 0; i < unboarded_size; i++)
	{
		m_unboarded[i]->Draw(screen);
	}
}