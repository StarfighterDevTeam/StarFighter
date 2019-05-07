#include "CrewInterface.h"

extern Game* CurrentGame;

CrewInterface::CrewInterface()
{
	m_crew = NULL;

	m_panel = NULL;
	m_portrait = NULL;
	m_lifebar = NULL;

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		m_skillbars[i] = NULL;
	}
}

CrewInterface::~CrewInterface()
{
	delete m_panel;
	delete m_portrait;
	delete m_lifebar;

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		delete m_skillbars[i];
	}

	m_crew = NULL;

	m_panel = NULL;
	m_portrait = NULL;
	m_lifebar = NULL;

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		m_skillbars[i] = NULL;
	}
}

void CrewInterface::Destroy()
{
	delete m_panel;
	delete m_portrait;
	delete m_lifebar;
	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		delete m_skillbars[i];
	}

	m_crew = NULL;

	m_panel = NULL;
	m_portrait = NULL;
	m_lifebar = NULL;

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		m_skillbars[i] = NULL;
	}
}

void CrewInterface::Init(CrewMember* crew)
{
	m_crew = crew;
	m_position = sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y * 0.5f);

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CREWINTERFACE_SIZE_X, CREWINTERFACE_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f, CREWINTERFACE_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color(166, 138, 117, 255));//beige "paper"
	m_panel->m_shape_container.setOutlineThickness(2.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y * 0.5f));

	//name
	float offset_y = 4.f;
	m_display_name.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_display_name.setCharacterSize(14);
	m_display_name.setStyle(sf::Text::Bold);
	m_display_name.setColor(sf::Color::Black);
	m_display_name.setString(m_crew->m_display_name);
	m_display_name.setPosition(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f - m_display_name.getGlobalBounds().width * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y));

	//portrait
	m_portrait = new GameEntity(UI_None);
	Texture* texture = TextureLoader::getInstance()->loadTexture(m_crew->m_texture_big_name, CREWINTERFACE_PORTRAIT_SIZE, CREWINTERFACE_PORTRAIT_SIZE);
	m_portrait->setAnimation(texture, 1, 1);
	offset_y += texture->getSize().y * 0.5f + 28.f;
	m_portrait->m_position = sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y);
	m_portrait->setPosition(m_portrait->m_position);

	m_portrait->m_shape_container.setSize(sf::Vector2f(CREWINTERFACE_PORTRAIT_SIZE, CREWINTERFACE_PORTRAIT_SIZE));
	m_portrait->m_shape_container.setOrigin(sf::Vector2f(CREWINTERFACE_PORTRAIT_SIZE * 0.5f, CREWINTERFACE_PORTRAIT_SIZE * 0.5f));
	m_portrait->m_shape_container.setOutlineThickness(2.f);
	m_portrait->m_shape_container.setOutlineColor(sf::Color::Black);
	m_portrait->m_shape_container.setPosition(m_portrait->m_position);

	//type
	offset_y += texture->getSize().y * 0.5f + 6.f;
	m_type_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_type_text.setCharacterSize(16);
	m_type_text.setStyle(sf::Text::Italic);
	m_type_text.setColor(sf::Color::Black);
	m_type_text.setString((*CurrentGame).m_dico_crew_types[m_crew->m_type]);
	m_type_text.setPosition(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f - m_type_text.getGlobalBounds().width * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y));

	//race
	offset_y += 20.f;
	m_race_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_race_text.setCharacterSize(16);
	m_race_text.setStyle(sf::Text::Italic);
	m_race_text.setColor(sf::Color::Black);
	m_race_text.setString((*CurrentGame).m_dico_crew_races[m_crew->m_race]);
	m_race_text.setPosition(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f - m_race_text.getGlobalBounds().width * 0.5f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y));

	//lifebar
	offset_y += 40.f;
	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(CREWINTERFACE_LIFEBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(CREWINTERFACE_LIFEBAR_SIZE_X * 0.5f, CREWINTERFACE_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	//m_lifebar->m_shape_container.setOutlineThickness(1.f);
	//m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);
	m_lifebar->m_position = sf::Vector2f(CREWINTERFACE_LIFEBAR_SIZE_X * 0.5f + 4.f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y);
	m_lifebar->m_shape_container.setPosition(m_lifebar->m_position);

	m_lifebar->m_shape.setSize(sf::Vector2f(CREWINTERFACE_LIFEBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(CREWINTERFACE_LIFEBAR_SIZE_X * 0.5f, CREWINTERFACE_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setFillColor(sf::Color::Green);
	m_lifebar->m_shape.setPosition(m_lifebar->m_position);

	m_lifebar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_lifebar->m_text.setCharacterSize(14);
	m_lifebar->m_text.setStyle(sf::Text::Bold);
	m_lifebar->m_text.setColor(sf::Color::Black);
	m_lifebar->m_text.setString("??/??");
	m_lifebar->m_text.setPosition(sf::Vector2f(m_lifebar->m_position.x + m_lifebar->m_shape_container.getSize().x * 0.5f + CREWINTERFACE_TEXT_OFFSET_X, m_lifebar->m_position.y - m_lifebar->m_text.getGlobalBounds().height * 0.5f - 4.f));

	//status
	offset_y += 14.f;
	m_status_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_status_text.setCharacterSize(16);
	//m_status_text.setStyle(sf::Text::Bold);
	m_status_text.setColor(sf::Color::Black);
	m_status_text.setPosition(sf::Vector2f(0, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y));

	//skills
	offset_y += 12.f;
	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		offset_y += 10.f;
		m_skillbar_names[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_skillbar_names[i].setCharacterSize(16);
		m_skillbar_names[i].setStyle(sf::Text::Italic);
		m_skillbar_names[i].setColor(sf::Color::Black);
		m_skillbar_names[i].setString((*CurrentGame).m_dico_crew_skills[i]);
		m_skillbar_names[i].setPosition(sf::Vector2f(CREWINTERFACE_TEXT_OFFSET_X, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y));

		offset_y += 26.f;
		m_skillbars[i] = new GameEntity(UI_None);
		m_skillbars[i]->m_shape_container.setSize(sf::Vector2f(CREWINTERFACE_SKILLBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));
		m_skillbars[i]->m_shape_container.setOrigin(sf::Vector2f(CREWINTERFACE_SKILLBAR_SIZE_X * 0.5f, CREWINTERFACE_LIFEBAR_SIZE_Y * 0.5f));
		m_skillbars[i]->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
		//m_skillbars[i]->m_shape_container.setOutlineThickness(1.f);
		//m_skillbars[i]->m_shape_container.setOutlineColor(sf::Color::Black);
		m_skillbars[i]->m_position = sf::Vector2f(CREWINTERFACE_SKILLBAR_SIZE_X * 0.5f + 4.f, REF_WINDOW_RESOLUTION_Y - CREWINTERFACE_SIZE_Y + offset_y);
		m_skillbars[i]->m_shape_container.setPosition(m_skillbars[i]->m_position);

		m_skillbars[i]->m_shape.setSize(sf::Vector2f(CREWINTERFACE_SKILLBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));
		m_skillbars[i]->m_shape.setOrigin(sf::Vector2f(CREWINTERFACE_SKILLBAR_SIZE_X * 0.5f, CREWINTERFACE_LIFEBAR_SIZE_Y * 0.5f));
		m_skillbars[i]->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "skill"
		m_skillbars[i]->m_shape.setPosition(m_skillbars[i]->m_position);

		m_skillbars[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_skillbars[i]->m_text.setCharacterSize(14);
		m_skillbars[i]->m_text.setColor(sf::Color::Black);
	}
}

void CrewInterface::Update()
{
	if (m_crew == NULL)
	{
		return;
	}

	//life bar update
	int health = m_crew->m_health;
	Bound(health, 0, m_crew->m_health_max);

	float life_ratio = 1.0f * health / m_crew->m_health_max;

	m_lifebar->m_shape.setSize(sf::Vector2f(life_ratio * CREWINTERFACE_LIFEBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));

	float threshold[3] = { 0.7, 0.5, 0.3 };
	if (life_ratio >= threshold[1])
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Green);
	}
	else if (life_ratio >= threshold[2])
	{
		m_lifebar->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
	}
	else
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Red);
	}

	ostringstream ss_life;
	ss_life << m_crew->m_health << "/" << m_crew->m_health_max;
	m_lifebar->m_text.setString(ss_life.str());
	m_lifebar->m_text.setPosition(sf::Vector2f(m_lifebar->m_position.x + m_lifebar->m_shape_container.getSize().x * 0.5f + CREWINTERFACE_TEXT_OFFSET_X, m_lifebar->m_position.y - m_lifebar->m_text.getGlobalBounds().height * 0.5f - 4.f));

	//status
	string status = "";
	RoomTile* tile = m_crew->m_destination != NULL ? m_crew->m_destination : m_crew->m_tile;
	if (tile == NULL)
	{
		//do nothing
	}
	else if (tile->m_is_pierced == true && tile->m_health < tile->m_health_max)
	{
		status = "Repairing hull";
		m_status_text.setColor(sf::Color::Black);
	}
	else if (tile->m_system_tile != NULL && tile->m_system_tile->m_weapon != NULL && tile->m_system_tile->m_weapon->m_health > 0)
	{
		status = "Gunner";
		m_status_text.setColor(sf::Color::Red);
	}
	else
	{
		status = "Idle";
		m_status_text.setColor(sf::Color::Black);
	}

	m_status_text.setString(status);
	m_status_text.setPosition(sf::Vector2f(CREWINTERFACE_SIZE_X * 0.5f - m_status_text.getGlobalBounds().width * 0.5f, m_status_text.getPosition().y));

	//skills
	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		int skill_value = m_crew->m_skills[i];
		Bound(skill_value, 0, m_crew->m_skills_max[i]);

		float skill_ratio = 1.0f * skill_value / m_crew->m_skills_max[i];
		m_skillbars[i]->m_shape.setSize(sf::Vector2f(skill_ratio * CREWINTERFACE_SKILLBAR_SIZE_X, CREWINTERFACE_LIFEBAR_SIZE_Y));

		ostringstream ss_skill;
		ss_skill << m_crew->m_skills[i] << "%";
		m_skillbars[i]->m_text.setString(ss_skill.str());
		m_skillbars[i]->m_text.setPosition(sf::Vector2f(m_skillbars[i]->m_position.x + m_skillbars[i]->m_shape_container.getSize().x * 0.5f + 14.f, m_skillbars[i]->m_position.y - m_skillbars[i]->m_text.getGlobalBounds().height * 0.5f - 5.f));
	}
}

void CrewInterface::SetPosition(sf::Vector2f position)
{
	sf::Vector2f offset = position - m_position;

	m_panel->m_shape_container.setPosition(m_panel->m_shape_container.getPosition() + offset);
	m_display_name.setPosition(sf::Vector2f(m_display_name.getPosition() + offset));
	m_portrait->setPosition(m_portrait->getPosition() + offset);
	m_portrait->m_shape_container.setPosition(m_portrait->m_shape_container.getPosition() + offset);
	m_type_text.setPosition(m_type_text.getPosition() + offset);
	m_race_text.setPosition(sf::Vector2f(m_race_text.getPosition() + offset));
	m_lifebar->m_shape_container.setPosition(m_lifebar->m_shape_container.getPosition() + offset);
	m_lifebar->m_shape.setPosition(m_lifebar->m_shape.getPosition() + offset);
	m_lifebar->m_text.setPosition(m_lifebar->m_text.getPosition() + offset);
	m_status_text.setPosition(sf::Vector2f(m_status_text.getPosition() + offset));

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		m_skillbar_names[i].setPosition(m_skillbar_names[i].getPosition() + offset);
		m_skillbars[i]->m_shape_container.setPosition(m_skillbars[i]->m_shape_container.getPosition() + offset);
		m_skillbars[i]->m_shape.setPosition(m_skillbars[i]->m_shape.getPosition() + offset);
		m_skillbars[i]->m_text.setPosition(m_skillbars[i]->m_text.getPosition() + offset);
	}

	m_position = position;
}

void CrewInterface::Draw(sf::RenderTexture& screen)
{
	if (m_crew == NULL)
	{
		return;
	}

	m_panel->Draw(screen);
	screen.draw(m_display_name);
	m_portrait->Draw(screen);
	screen.draw(m_type_text);
	screen.draw(m_race_text);
	m_lifebar->Draw(screen);
	screen.draw(m_status_text);

	for (int i = 0; i < NB_CREW_SKILLS; i++)
	{
		screen.draw(m_skillbar_names[i]);
		m_skillbars[i]->Draw(screen);
	}
}