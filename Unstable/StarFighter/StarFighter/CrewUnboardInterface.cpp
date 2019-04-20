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

void CrewUnboardInterface::Init(Ship* ship, Location* location)
{
	if (ship == NULL || location == NULL)
	{
		return;
	}

	m_ship = ship;
	m_location = location;
	m_slots_avaible = ship->m_lifeboats;

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
	ostringstream ss_unboarders;
	switch (location->m_type)
	{
		case Location_Seaport:
		{
			ss_unboarders << "Your lifeboats allow for " << m_slots_avaible << " crew members to unboard.\nRight click on crew members to select them for the mission.";
			break;
		}
		case Location_SeaMonster:
		case Location_Wreck:
		{
			ss_unboarders << "A massive object is detected by your sonar underneath.\nYour diving suits allow for " << m_slots_avaible << " crew members to dive.\nRight click on crew members to select them if you want to risk the dive.";
			break;
		}
	}
	
	float offset_y = m_panel->m_position.y - CREWUNBOARDINTERFACE_SIZE_Y * 0.5;
	offset_y += 20;
	m_narrative_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_narrative_text.setCharacterSize(18);
	m_narrative_text.setStyle(sf::Text::Bold);
	m_narrative_text.setColor(sf::Color::White);
	m_narrative_text.setString(ss_unboarders.str());
	m_narrative_text.setPosition(sf::Vector2f(m_panel->m_position.x - CREWUNBOARDINTERFACE_SIZE_X * 0.5 + 20, offset_y));

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

	//choices
	offset_y += CHOICE_PANEL_SIZE_Y * 0.5f + 30;
	for (int i = 0; i < NB_CHOICES_MAX; i++)
	{
		if (m_location->m_choicesID[i] < 0)
		{
			break;
		}
		
		m_choices[i].Init(i, m_location->m_choicesID[i]);
		m_choices[i].SetPosition(sf::Vector2f(prisoners_offset_x + CHOICE_PANEL_SIZE_X * 0.5f + 50 + CREWINTERFACE_SIZE_X, offset_y + (i * CHOICE_PANEL_SIZE_Y)));
	}
}

Reward* CrewUnboardInterface::Update(sf::Time deltaTime)
{
	//crew slots
	ostringstream ss_slots;
	ss_slots << m_unboarded.size() << " / " << m_slots_avaible;
	m_crew_slot_text.setString(ss_slots.str());

	int k = 0;
	for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
	{
		(*it)->m_position = m_crew_slots[k]->m_shape_container.getPosition();
		(*it)->Update(deltaTime);
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
		if (m_location->m_choicesID[i] < 0)
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
		if (m_choices[i].m_skill > 0 && m_choices[i].m_gauge_value == 0)
		{
			m_choices[i].m_picture->setAnimationLine(1);
			continue;
		}

		//click = action
		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			//pay "days" cost
			m_ship->PayUpkeepCost(m_choices[i].m_cost[Resource_Days]);

			//randomize reward ID
			int rewardID = m_choices[i].RandomizeRewardID();
			
			//read reward in the database
			if (rewardID > 0)
			{
				rewardID--;
				Reward* reward = new Reward();
				int k = 0;
				for (int j = 0; j < NB_RESOURCES_TYPES; j++)
				{
					//pay cost
					if (j != Resource_Days && j != Resource_Fidelity)
					{
						int cost = stoi((*CurrentGame).m_choices_config[m_location->m_choicesID[i] - 1][Choice_CostGold + j]);
						m_ship->AddResource(ResourceType(Resource_Gold + j), -cost);
					}

					//get rewards
					int value = stoi((*CurrentGame).m_rewards_config[rewardID][Reward_Gold + j]);

					if (value == 0)
					{
						continue;
					}

					if (j == Resource_Gold || j == Resource_Fish || j == Resource_Mech)
					{
						//add random + pro rata of skills invested (if any)
						float cooldown = m_location->m_visited_countdown == 0 ? 1.f : (1.f * m_location->m_visited_countdown / RESOURCES_REFRESH_RATE_IN_DAYS);

						if (m_choices[i].m_gauge_value_max == 0)
						{
							value = (int)(1.f * value * RandomizeFloatBetweenValues(0.8, 1.2) * cooldown);
						}
						else
						{
							value = (int)(1.f * value * (1.f * m_choices[i].m_gauge_value / m_choices[i].m_gauge_value_max) * RandomizeFloatBetweenValues(0.8, 1.2) * cooldown);
						}
					}

					pair<ResourceType, int> resource;
					resource.first = (ResourceType)j;
					resource.second = value;
					reward->m_resources.push_back(resource);

					k++;
				}

				//secret wreck location
				int secret = stoi((*CurrentGame).m_rewards_config[rewardID][Reward_SecretWreck]);
				if (secret == 1)
				{
					reward->m_DMS_location = new DMS_Coord();
					k++;
				}

				//reward text
				reward->m_string = (*CurrentGame).m_rewards_config[rewardID][Reward_Text];
				reward->m_string = StringReplace(reward->m_string, "_", " ");
				reward->m_string = StringCut(reward->m_string, 48);

				return reward;
			}
			else
			{
				return new Reward();
			}
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
		if (m_location->m_choicesID[i] < 0)
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
		bool found = false;
		for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
		{
			if (crew == (*it)->m_clone)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			CrewMember* clone = crew->Clone();
			m_unboarded.push_back(clone);
			clone->m_UI_type = UI_CrewMemberUnboarding;
			clone->m_clone = crew;
		}

		return found == false;
	}
	else
	{
		return false;
	}
}

void CrewUnboardInterface::RemoveCrewFromInterface(CrewMember* crew)
{
	//crew->m_UI_type = UI_CrewMember;

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
	/*
	vector<CrewMember*> old_crew;
	for (vector<CrewMember*>::iterator it = m_unboarded.begin(); it != m_unboarded.end(); it++)
	{
		old_crew.push_back(*it);
	}
	m_unboarded.clear();
	for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if (*it != crew)
		{
			m_unboarded.push_back(*it);
		}
	}


	if (m_crew_interface.m_crew == crew)
	{
		m_crew_interface.Destroy();
	}
	*/

}