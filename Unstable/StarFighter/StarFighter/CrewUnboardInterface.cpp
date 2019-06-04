#include "CrewUnboardInterface.h"

extern Game* CurrentGame;

CrewUnboardInterface::CrewUnboardInterface()
{
	m_panel = NULL;
	m_hovered = NULL;

	m_ship = NULL;
	m_location = NULL;
}

CrewUnboardInterface::~CrewUnboardInterface()
{
	//delete m_panel;
	//delete m_drowning_bar;

	Destroy();
}

void CrewUnboardInterface::Destroy()
{
	delete m_panel;

	m_panel = NULL;

	for (int i = 0; i < m_crew_slots.size(); i++)
	{
		delete m_crew_slots[i];
	}
	m_crew_slots.clear();

	m_ship = NULL;
	m_location = NULL;
	m_other_ship = NULL;
	m_slots_avaible = 0;

	for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
	{
		delete *it;
	}
	m_unboarded.clear();

	m_crew_interface.Destroy();

	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		m_choices[i].Destroy();
	}
}

void CrewUnboardInterface::Init(Ship* ship, Location* location, Ship* other_ship)
{
	m_ship = ship;
	m_location = location;
	m_other_ship = other_ship;
	if (location != NULL && location->m_type == Location_Seaport)
	{
		m_slots_avaible = ship->m_lifeboats;
	}
	else if (location != NULL)
	{
		m_slots_avaible = ship->m_diving_suits;
	}

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CREWUNBOARDINTERFACE_SIZE_X, CREWUNBOARDINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CREWUNBOARDINTERFACE_SIZE_X * 0.5, CREWUNBOARDINTERFACE_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - CREWUNBOARDINTERFACE_SIZE_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	
	//narrative text
	ostringstream ss_narrative;
	if (location != NULL)
	{
		switch (location->m_type)
		{
			case Location_Seaport:
			{
				ss_narrative << "Your lifeboats allow for " << m_slots_avaible << " crew members to unboard.\nRight click on crew members to select them for the mission.";
				break;
			}
			case Location_SeaMonster:
			case Location_Wreck:
			{
				ss_narrative << "A massive object is detected by your sonar at " << location->m_depth << "m underneath.\nYour diving suits allow for " << m_slots_avaible << " crew members to dive.\nRight click on crew members to select them if you want to risk the dive.";
				break;
			}
			case Location_Fish:
			{
				ss_narrative << "Your sonar detects something moving at " << location->m_depth << "m underneath.\nYour diving suits allow for " << m_slots_avaible << " crew members to dive.\nRight click on crew members to select them if you want to risk the dive.";
				break;
			}
		}
	}
	else if (other_ship != NULL)
	{
		if (other_ship->m_alliance == Alliance_Enemy)
		{
			ss_narrative << "You approach a " << (*CurrentGame).m_dico_ship_class[other_ship->m_type] << " ship belonging to the Royal Navy.\nThe '" << other_ship->m_display_name.c_str() << "' shows aggressive intentions towards you.";
		}
		else if (other_ship->m_alliance == Alliance_Ally)
		{
			ss_narrative << "You approach a commercial ship that doesn't seem like a threat.\nOn the hull, you can read the name '" << other_ship->m_display_name.c_str() << "'.";
		}
	}
	
	float offset_y = m_panel->m_position.y - CREWUNBOARDINTERFACE_SIZE_Y * 0.5;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_narrative.str());
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5 + 20, offset_y));

	float prisoners_offset_x = m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5f + 20;
	offset_y += 90;
	
	//crew unboarding
	if (location != NULL)
	{
		//crew slots / slots available
		
		ostringstream ss_slots;
		ss_slots << 0 << " / " << m_slots_avaible;
		m_crew_slot_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_crew_slot_text.setCharacterSize(18);
		m_crew_slot_text.setStyle(sf::Text::Bold);
		m_crew_slot_text.setColor(sf::Color::White);
		m_crew_slot_text.setString(ss_slots.str());
		m_crew_slot_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5f + 20, offset_y - m_crew_slot_text.getCharacterSize() * 0.5f));

		//crew slots
		offset_y += m_crew_slot_text.getCharacterSize() + 20;
		int crew_per_line = 10;
		
		for (int i = 0; i < m_slots_avaible; i++)
		{
			float pos_x = prisoners_offset_x + (CREWMEMBER_SIZE * 0.5) + ((i % crew_per_line) * (CREWMEMBER_SIZE + 10));
			float pos_y = offset_y + ((i / crew_per_line) * (CREWMEMBER_SIZE + 10));
			offset_y = Maxf(offset_y, pos_y);

			GameEntity* crew_slot = new GameEntity(UI_None);
			crew_slot->m_shape_container.setSize(sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE));
			crew_slot->m_shape_container.setOrigin(sf::Vector2f(CREWMEMBER_SIZE * 0.5, CREWMEMBER_SIZE * 0.5));
			crew_slot->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
			crew_slot->m_shape_container.setOutlineThickness(2);
			crew_slot->m_shape_container.setOutlineColor(sf::Color::Black);
			crew_slot->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
			crew_slot->m_position = crew_slot->m_shape_container.getPosition();
			m_crew_slots.push_back(crew_slot);
		}
	}

	//choices
	offset_y += CHOICE_PANEL_SIZE_Y * 0.5f + 30;
	string choicesID[NB_CHOICES_MAX];
	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		if (location != NULL)
		{
			choicesID[i] = location->m_choicesID[i];
		}
		else if (other_ship != NULL)
		{
			choicesID[i] = other_ship->m_choicesID[i];
		}
	}

	int k = 0;
	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		//has a valid choice ID?
		if (choicesID[i].empty() == true)
		{
			break;
		}

		bool ok_to_continue = false;
		int line = 0;
		for (vector<vector <string> >::iterator it = (*CurrentGame).m_choices_config.begin(); it != (*CurrentGame).m_choices_config.end(); it++)
		{
			if ((*it).front().compare(choicesID[i]) == 0)
			{
				ok_to_continue = ship->HasCommodity((CommodityType)stoi((*CurrentGame).m_choices_config[line][Choice_CommodityRequired])) == true;
				break;
			}
			else
			{
				line++;
			}
		}

		if (ok_to_continue == false)
		{
			continue;
		}

		m_choices[k].Init(k, choicesID[i]);
		m_choices[k].SetPosition(sf::Vector2f(prisoners_offset_x + CHOICE_PANEL_SIZE_X * 0.5f + 50 + CREWINTERFACE_SIZE_X, offset_y + (k * CHOICE_PANEL_SIZE_Y)));
		k++;
	}
}

Choice* CrewUnboardInterface::Update(sf::Time deltaTime)
{
	//crew slots
	ostringstream ss_slots;
	ss_slots << m_unboarded.size() << " / " << m_slots_avaible;
	m_crew_slot_text.setString(ss_slots.str());

	int k = 0;
	for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
	{
		(*it)->m_position = m_crew_slots[k]->m_shape_container.getPosition();
		(*it)->setFrame(1);
		(*it)->GameEntity::Update(deltaTime);
		(*it)->m_crew_linked->setColor(sf::Color(255, 255, 255, 120));
		k++;

		if ((*it)->IsHoveredByMouse() == true && m_crew_interface.m_crew != *it)
		{
			//prisoner card (focus)
			if (m_crew_interface.m_crew != NULL)
			{
				m_crew_interface.Destroy();
			}

			m_crew_interface.Init((*it));
			m_crew_interface.Update();

			float pos_x = m_panel->m_position.x - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f + m_crew_interface.m_panel->m_shape_container.getSize().x * 0.5f + 20;
			m_crew_interface.SetPosition(sf::Vector2f(pos_x, (m_choices[0].m_picture->getPosition().y - CHOICE_PANEL_SIZE_Y * 0.5 + CREWINTERFACE_SIZE_Y * 0.5)));// +m_crew_interface.m_panel->m_shape_container.getSize().y * 0.5 - CREWMEMBER_SIZE * 0.5));
		}
		else if ((*it)->IsHoveredByMouse() == false && m_crew_interface.m_crew == *it)
		{
			m_crew_interface.Destroy();
		}
	}

	//choices
	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		if (m_choices[i].m_ID.empty() == true)
		{
			break;
		}

		//update gauge
		int crew_size = m_unboarded.size();
		int gauge_value = 0;
		if (m_choices[i].m_skill >= 0)
		{
			for (int j = 0; j < crew_size; j++)
			{
				gauge_value += m_unboarded[j]->m_skills[m_choices[i].m_skill];
			}
		}

		//is choice hovered?
		if (m_choices[i].Update(gauge_value) == false)
		{
			continue;
		}
		
		//checking conditions
		bool skill_missing = m_choices[i].m_skill > 0 && m_choices[i].m_gauge_value == 0;
		bool commodity_missing = m_choices[i].m_cost_commodity != Commodity_None && m_ship->HasCommodity(m_choices[i].m_cost_commodity) == false;
		if (skill_missing == true || commodity_missing == true)
		{
			m_choices[i].m_picture->setAnimationLine(1);
			continue;
		}

		//click = action
		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			return &m_choices[i];			
		}
	}

	return NULL;
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

	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		if (m_choices[i].m_ID.empty() == true)
		{
			break;
		}

		m_choices[i].Draw(screen);
	}

	if (m_crew_interface.m_crew != NULL)
	{
		m_crew_interface.Draw(screen);
	}
}

bool CrewUnboardInterface::AddCrewToInterface(CrewMember* crew)
{
	if (crew->m_is_prisoner == false && m_unboarded.size() < m_slots_avaible)
	{
		for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
		{
			if (crew == (*it)->m_crew_linked)
			{
				return false;
			}
		}

		CrewMember* clone = crew->Clone();
		clone->setAnimationLine(AnimationDirection_Down + (NB_ANIMATION_DIRECTIONS * clone->m_skin));
		m_unboarded.push_back(clone);
		clone->m_UI_type = UI_CrewMemberUnboarding;
		clone->m_crew_linked = crew;
		return true;
	}
	else
	{
		return false;
	}
}

void CrewUnboardInterface::RemoveCrewFromInterface(CrewMember* crew)
{
	//remove from interface
	for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
	{
		if (crew == *it)
		{
			if (m_crew_interface.m_crew == crew)
			{
				m_crew_interface.Destroy();
			}

			m_unboarded.erase(it);
			break;
		}
	}
}