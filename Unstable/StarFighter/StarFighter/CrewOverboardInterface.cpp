#include "CrewOverboardInterface.h"

extern Game* CurrentGame;

CrewOverboardInterface::CrewOverboardInterface()
{
	m_panel = NULL;
	m_drowning_bar = NULL;
	m_ship = NULL;
}

CrewOverboardInterface::~CrewOverboardInterface()
{
	delete m_panel;
	delete m_drowning_bar;
}

void CrewOverboardInterface::Destroy()
{
	delete m_panel;
	delete m_drowning_bar;

	m_panel = NULL;
	m_drowning_bar = NULL;
	m_crew.clear();

	for (int i = 0; i < m_slots_avaible; i++)
	{
		delete m_crew_slots[i];
	}
	m_crew_slots.clear();
}

void CrewOverboardInterface::Init(Ship* ship, Ship* enemy_ship)
{
	m_slots_avaible = 4; //todo: get lifeboat numbers from ship*
	m_drowning_timer = OVERBOARD_DROWNING_TIME;

	//get prisoners among survivors
	int crew_size = 0;
	for (vector<CrewMember*>::iterator it = enemy_ship->m_crew.begin(); it != enemy_ship->m_crew.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			(*it)->m_tile = NULL;
			(*it)->m_destination = NULL;
			(*it)->m_speed = sf::Vector2f(0, 0);
			(*it)->m_melee_opponent = NULL;
			crew_size++;
		}
	}

	for (vector<CrewMember*>::iterator it = enemy_ship->m_prisoners.begin(); it != enemy_ship->m_prisoners.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			(*it)->m_tile = NULL;
			(*it)->m_destination = NULL;
			(*it)->m_speed = sf::Vector2f(0, 0);
			(*it)->m_melee_opponent = NULL;
			crew_size++;
		}
	}

	//no survivor = no need for a menu
	if (crew_size == 0)
	{
		return;
	}

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CREWOVERBOARDINTERFACE_SIZE_X, CREWOVERBOARDINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CREWOVERBOARDINTERFACE_SIZE_X * 0.5f, CREWOVERBOARDINTERFACE_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f - CREWOVERBOARDINTERFACE_OFFSET_X, CREWOVERBOARDINTERFACE_SIZE_Y * 0.5f + CREWOVERBOARDINTERFACE_OFFSET_Y));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//narrative text
	ostringstream ss_survivors;
	ss_survivors << "There are " << crew_size << " survivors overboard.\nYour lifeboats allow for " << m_slots_avaible << " rescue.\nRight-click on survivors to rescue.";
	float offset_y = m_panel->m_position.y - CREWOVERBOARDINTERFACE_SIZE_Y * 0.5f;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_survivors.str());
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f + 20, offset_y));

	//drowning bar
	offset_y += 100;
	m_drowning_bar = new GameEntity(UI_None);
	m_drowning_bar->m_shape_container.setSize(sf::Vector2f(OVERBOARD_DROWNINGBAR_SIZE_X, OVERBOARD_DROWNINGBAR_SIZE_Y));
	m_drowning_bar->m_shape_container.setOrigin(sf::Vector2f(OVERBOARD_DROWNINGBAR_SIZE_X * 0.5f, OVERBOARD_DROWNINGBAR_SIZE_Y * 0.5f));
	m_drowning_bar->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Grey_Background]);
	m_drowning_bar->m_shape_container.setOutlineThickness(2.f);
	m_drowning_bar->m_shape_container.setOutlineColor(sf::Color::Black);
	m_drowning_bar->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f + OVERBOARD_DROWNINGBAR_SIZE_X * 0.5f + 80, offset_y));
	m_drowning_bar->m_position = m_drowning_bar->m_shape_container.getPosition();

	m_drowning_bar->m_shape.setSize(sf::Vector2f(OVERBOARD_DROWNINGBAR_SIZE_X, OVERBOARD_DROWNINGBAR_SIZE_Y));
	m_drowning_bar->m_shape.setOrigin(sf::Vector2f(OVERBOARD_DROWNINGBAR_SIZE_X * 0.5f, OVERBOARD_DROWNINGBAR_SIZE_Y * 0.5f));
	m_drowning_bar->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Pierced]);
	m_drowning_bar->m_shape.setOutlineThickness(2.f);
	m_drowning_bar->m_shape.setOutlineColor(sf::Color::Black);
	m_drowning_bar->m_shape.setPosition(sf::Vector2f(m_panel->m_position.x - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f + OVERBOARD_DROWNINGBAR_SIZE_X * 0.5f + 80, offset_y));
	m_drowning_bar->m_position = m_drowning_bar->m_shape_container.getPosition();

	ostringstream ss_slots;
	ss_slots << 0 << " / " << m_slots_avaible;
	m_drowning_bar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_drowning_bar->m_text.setCharacterSize(18);
	m_drowning_bar->m_text.setStyle(sf::Text::Bold);
	m_drowning_bar->m_text.setColor(sf::Color::White);
	m_drowning_bar->m_text.setString(ss_slots.str());
	m_drowning_bar->m_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f + 20, offset_y - m_drowning_bar->m_text.getCharacterSize() * 0.5f));

	//prisoners
	offset_y += OVERBOARD_DROWNINGBAR_SIZE_Y + 22;
	int prisoners_per_line = 10;
	float prisoners_offset_x = m_panel->m_position.x - CREWOVERBOARDINTERFACE_SIZE_X * 0.5f + 20;
	for (int i = 0; i < m_slots_avaible; i++)
	{
		float pos_x = prisoners_offset_x + (CREWMEMBER_SIZE * 0.5f) + ((i % prisoners_per_line) * (CREWMEMBER_SIZE + 10));
		float pos_y = offset_y + ((i / prisoners_per_line) * (CREWMEMBER_SIZE + 10));
		offset_y = Maxf(offset_y, pos_y);

		GameEntity* crew_slot = new GameEntity(UI_None);
		crew_slot->m_shape_container.setSize(sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE));
		crew_slot->m_shape_container.setOrigin(sf::Vector2f(CREWMEMBER_SIZE * 0.5f, CREWMEMBER_SIZE * 0.5f));
		crew_slot->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Grey_Background]);
		crew_slot->m_shape_container.setOutlineThickness(2.f);
		crew_slot->m_shape_container.setOutlineColor(sf::Color::Black);
		crew_slot->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
		crew_slot->m_position = m_drowning_bar->m_shape_container.getPosition();
		m_crew_slots.push_back(crew_slot);
	}
}

void CrewOverboardInterface::Update(sf::Time deltaTime)
{
	if (m_drowning_timer > 0)
	{
		m_drowning_timer -= deltaTime.asSeconds();
		m_drowning_timer = Maxf(m_drowning_timer, 0.f);

		float drowning_ratio = m_drowning_timer == 0 ? 0 : m_drowning_timer / OVERBOARD_DROWNING_TIME;
		m_drowning_bar->m_shape.setSize(sf::Vector2f(drowning_ratio * OVERBOARD_DROWNINGBAR_SIZE_X, OVERBOARD_DROWNINGBAR_SIZE_Y));
	}

	if (m_drowning_timer <= 0)
	{
		printf("");
		//exit;
	}	
}

void CrewOverboardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);
	m_drowning_bar->Draw(screen);
	screen.draw(m_narrative_text);

	int crew_size = m_crew_slots.size();
	for (int i = 0; i < crew_size; i++)
	{
		m_crew_slots[i]->Draw(screen);
	}
}