#include "PrisonersChoiceInterface.h"

extern Game* CurrentGame;

PrisonersChoiceInterface::PrisonersChoiceInterface()
{
	m_panel = NULL;
	m_crew_selected = NULL;
	m_crew_hovered = NULL;
	m_current_choice = 0;
	m_ship = NULL;
}

PrisonersChoiceInterface::~PrisonersChoiceInterface()
{
	//delete m_panel;
	Destroy();
}

void PrisonersChoiceInterface::Destroy()
{
	m_crew_interface.Destroy();

	m_crew_selected = NULL;
	m_crew_hovered = NULL;

	delete m_panel;
	for (int i = 0; i < 3; i++)
	{
		m_choices[i].Destroy();
	}

	m_panel = NULL;
	m_crew_selected = NULL;
	m_crew_hovered = NULL;
	m_current_choice = 0;
	m_crew.clear();
}

void PrisonersChoiceInterface::Init(Ship* ship, Ship* enemy_ship)
{
	m_ship = ship;

	//get prisoners among survivors
	for (int j = 0; j < 2; j++)
	{
		for (vector<CrewMember*>::iterator it = enemy_ship->m_crew[j].begin(); it != enemy_ship->m_crew[j].end(); it++)
		{
			if ((*it)->m_health > 0)
			{
				m_crew.push_back(*it);
				(*it)->m_tile = NULL;
				(*it)->m_destination = NULL;
				(*it)->m_speed = sf::Vector2f(0, 0);
				(*it)->m_melee_opponent = NULL;
			}
		}
		enemy_ship->m_crew[j].clear();
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
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
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
	int crew_per_line = 10;
	float prisoners_offset_x = m_panel->m_position.x - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f + 230;
	for (int i = 0; i < crew_size; i++)
	{
		float pos_x = prisoners_offset_x + (CREWMEMBER_SIZE * 0.5f) + ((i % crew_per_line) * (CREWMEMBER_SIZE + 10));
		float pos_y = offset_y + ((i / crew_per_line) * (CREWMEMBER_SIZE + 10));
		offset_y = Maxf(offset_y, pos_y);
		m_crew[i]->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
	}

	//prisoner card (focus)
	m_crew_selected = m_crew.front();
	m_crew_interface.Init(m_crew_selected);
	m_crew_interface.Update();
	m_crew_selected->m_shape_container.setOutlineThickness(2.f);
	m_crew_selected->m_shape_container.setOutlineColor(sf::Color::Green);

	float pos_x = m_panel->m_position.x - PRISONERSCHOICEINTERFACE_SIZE_X * 0.5f + m_crew_interface.m_panel->m_shape_container.getSize().x * 0.5f + 20;
	m_crew_interface.SetPosition(sf::Vector2f(pos_x, offset_y + m_crew_interface.m_panel->m_shape_container.getSize().y * 0.5f - CREWMEMBER_SIZE * 0.5f));
	
	//choices
	offset_y += CHOICE_PANEL_SIZE_Y * 0.5f + 50.f;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			m_choices[i].Init(i, "", "A la planche !", "2D/choice_prisoner_1.png");
		}
		else if (i == 1)
		{
			m_choices[i].Init(i, "", "En prison.", "2D/choice_prisoner_2.png");
		}
		else if (i == 2)
		{
			m_choices[i].Init(i, "", "Enrôler dans votre équipage.", "2D/choice_prisoner_3.png");
		}
		
		m_choices[i].SetPosition(sf::Vector2f(prisoners_offset_x + CHOICE_PANEL_SIZE_X * 0.5f, offset_y + (i * CHOICE_PANEL_SIZE_Y)));
	}
}

void PrisonersChoiceInterface::Update(sf::Time deltaTime)
{
	for (int i = 0; i < NB_CHOICES_MAX - 1; i++)
	{
		if (m_choices[i].Update(0) == false)//is choice hovered?
		{
			continue;
		}
		
		//checking conditions
		bool condition = true;
		if (i == 1)//imprison
		{
			condition = m_ship->IsPrisonCellFree() == true;
		}
		else if (i == 2)//hiring crew
		{
			condition = m_ship->m_nb_crew < m_ship->m_nb_crew_max;
		}

		if (condition == false)
		{
			m_choices[i].m_picture->setAnimationLine(1);
			break;
		}

		//click = action
		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			sf::Vector2f prisoner_offset = m_crew_selected->m_shape_container.getPosition();
			if (m_crew_selected == m_crew_hovered)
			{
				m_crew_hovered = NULL;
			}

			if (i == 0)//kill crew
			{
				delete m_crew_selected;
			}
			else if (i == 1)//imprison
			{
				if (m_ship->AddNewPrisoner(m_crew_selected) == false)
					break;
			}
			else if (i == 2)//hire
			{
				m_ship->AddNewCrewMember(m_crew_selected);
			}

			//update prisoners list
			if (m_crew.size() > 1)
			{
				vector<CrewMember*> old_crew;
				for (vector<CrewMember*>::iterator it = m_crew.begin(); it != m_crew.end(); it++)
				{
					old_crew.push_back(*it);
				}
				m_crew.clear();
				int j = -1;
				for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
				{
					j++;
					if (j == 0)
					{
						continue;
					}
					m_crew.push_back(*it);
				}

				m_crew_selected = m_crew.front();
				sf::Vector2f position = m_crew_interface.m_position;
				m_crew_interface.Init(m_crew_selected);
				m_crew_interface.Update();
				m_crew_interface.SetPosition(position);
				m_crew_selected->m_shape_container.setOutlineThickness(2.f);
				m_crew_selected->m_shape_container.setOutlineColor(sf::Color::Green);

				//reorder remaining prisoners
				int crew_size = m_crew.size();
				int prisoners_per_line = 10;
				for (int i = 0; i < crew_size; i++)
				{
					float pos_x = prisoner_offset.x + ((i % prisoners_per_line) * (CREWMEMBER_SIZE + 10));
					float pos_y = prisoner_offset.y + ((i / prisoners_per_line) * (CREWMEMBER_SIZE + 10));
					m_crew[i]->m_shape_container.setPosition(sf::Vector2f(pos_x, pos_y));
				}

				break;
			}
			else
			{
				m_crew.clear();
			}
		}
	}

	//create prisoner card for hovered prisoner
	int crew_size = m_crew.size();
	if (crew_size > 1)
	{
		bool was_hovered = m_crew_hovered != NULL;
		bool is_hovered = false;

		for (int i = 0; i < crew_size; i++)
		{
			if (m_crew[i]->IsHoveredByMouse() == true)
			{
				m_crew_hovered = m_crew[i];
				sf::Vector2f position = m_crew_interface.m_position;
				m_crew_interface.Init(m_crew_hovered);
				m_crew_interface.Update();
				m_crew_interface.SetPosition(position);
				is_hovered = true;
				m_crew_hovered->m_shape_container.setOutlineThickness(2.f);
				m_crew_hovered->m_shape_container.setOutlineColor(sf::Color::Red);
			}
			else if (m_crew[i] != m_crew_selected)
			{
				m_crew[i]->m_shape_container.setOutlineColor(m_crew[i]->m_default_color);
				m_crew[i]->m_shape_container.setOutlineThickness(-1.f);
			}
			else
			{
				m_crew[i]->m_shape_container.setOutlineColor(sf::Color::Green);
				m_crew[i]->m_shape_container.setOutlineThickness(2.f);
			}
		}

		//recreate prisoner card selected after hovering is over
		if (was_hovered == true && is_hovered == false)
		{
			sf::Vector2f position = m_crew_interface.m_position;
			m_crew_interface.Init(m_crew_selected);
			m_crew_interface.Update();
			m_crew_interface.SetPosition(position);
		}
	}
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

	for (int i = 0; i < 3; i++)
	{
		m_choices[i].Draw(screen);
	}
}