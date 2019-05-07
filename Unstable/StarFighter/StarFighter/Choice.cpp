#include "Choice.h"

extern Game* CurrentGame;

Choice::Choice()
{
	m_panel = NULL;
	m_picture = NULL;
	m_gauge = NULL;

	m_ID = -1;
}

Choice::~Choice()
{
	Destroy();
}

void Choice::Destroy()
{
	delete m_panel;
	delete m_picture;
	delete m_gauge;

	for (vector<GameEntity*>::iterator it = m_costs_displayed.begin(); it != m_costs_displayed.end(); it++)
	{
		delete (*it);
	}
	m_costs_displayed.clear();

	m_panel = NULL;
	m_picture = NULL;
	m_gauge = NULL;

	m_ID = -1;
}

void Choice::Init(int index, int choiceID, string text, string portrait_filename)
{
	m_ID = choiceID;

	//Init values
	m_skill = -1;
	m_gauge_value_max = 0;
	m_gauge_value = 0;
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		m_cost[i] = 0;
	}
	m_cost_commodity = Commodity_None;

	//Load from an ID?
	if (choiceID > 0)
	{
		choiceID--;//ID 1 starts at line 0
		text = (*CurrentGame).m_choices_config[choiceID][Choice_Text];
		text = StringReplace(text, "_", " ");

		portrait_filename = (*CurrentGame).m_choices_config[choiceID][Choice_Picturename];

		for (int i = 0; i < NB_RESOURCES_TYPES; i++)
		{
			m_cost[i] = stoi((*CurrentGame).m_choices_config[choiceID][Choice_CostGold + i]);
		}
		m_skill = stoi((*CurrentGame).m_choices_config[choiceID][Choice_Skill]);
		m_gauge_value_max = stoi((*CurrentGame).m_choices_config[choiceID][Choice_ValueMax]);

		for (int i = 0; i < NB_CHOICE_REWARDS_MAX; i++)
		{
			m_rewardsID[i].first = stoi((*CurrentGame).m_choices_config[choiceID][Choice_Reward1 + (i * 2)]);
			m_rewardsID[i].second = stoi((*CurrentGame).m_choices_config[choiceID][Choice_Reward1_Proba + (i * 2)]);
		}

		m_cost_commodity = (CommodityType)stoi((*CurrentGame).m_choices_config[choiceID][Choice_CostCommodity]);
	}

	//Start building interface
	m_position = sf::Vector2f(0, 0);

	//background
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(CHOICE_PANEL_SIZE_X, CHOICE_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(CHOICE_PANEL_SIZE_X * 0.5f, CHOICE_PANEL_SIZE_Y * 0.5f));
	m_panel->m_shape_container.setFillColor(sf::Color::White);
	m_panel->m_shape_container.setOutlineThickness(4.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(0.f, 0.f));

	//shortcut index of the choice "1.", "2.", etc...
	ostringstream ss_index;
	ss_index << index + 1 << ".";
	m_panel->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_panel->m_text.setCharacterSize(20);
	m_panel->m_text.setColor(sf::Color::White);
	m_panel->m_text.setString(ss_index.str());
	m_panel->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5f - 34, 0.f - m_panel->m_text.getGlobalBounds().height * 0.5);

	//texture container
	m_picture = new GameEntity(UI_None);
	m_picture->m_shape_container.setSize(sf::Vector2f(CHOICE_VIDEO_SIZE_X, CHOICE_PANEL_SIZE_Y));
	m_picture->m_shape_container.setOrigin(sf::Vector2f(CHOICE_VIDEO_SIZE_X * 0.5f, CHOICE_PANEL_SIZE_Y * 0.5f));
	m_picture->m_shape_container.setFillColor(sf::Color::White);
	m_picture->m_shape_container.setOutlineThickness(4.f);
	m_picture->m_shape_container.setOutlineColor(sf::Color::Black);
	m_picture->m_shape_container.setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X * 0.5, 0.f));

	//texture
	Texture* texture = TextureLoader::getInstance()->loadTexture(portrait_filename, 280, 300);
	m_picture->setAnimation(texture, 1, 2);
	m_picture->setAnimationLine(1);
	m_picture->setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X * 0.5, 0.f));

	//title of the choice
	m_picture->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_picture->m_text.setCharacterSize(20);
	m_picture->m_text.setColor(sf::Color::Black);
	m_picture->m_text.setString(text);
	m_picture->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20, 0.f - CHOICE_PANEL_SIZE_Y * 0.5 + m_picture->m_text.getGlobalBounds().height * 0.5);

	//gauge (required skill + gauge)
	m_gauge = new GameEntity(UI_None);
	if (m_gauge_value_max > 0)
	{
		m_gauge->m_shape_container.setSize(sf::Vector2f(CHOICE_GAUGE_SIZE_X, CHOICE_GAUGE_SIZE_Y));
		m_gauge->m_shape_container.setOrigin(sf::Vector2f(CHOICE_GAUGE_SIZE_X * 0.5, CHOICE_GAUGE_SIZE_Y * 0.5f));
		m_gauge->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
		m_gauge->m_shape_container.setOutlineThickness(4.f);
		m_gauge->m_shape_container.setOutlineColor(sf::Color::Black);
		m_gauge->m_shape_container.setPosition(sf::Vector2f(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20 + CHOICE_GAUGE_SIZE_X * 0.5, 0.f - CHOICE_GAUGE_SIZE_Y * 0.5 + 50));

		m_gauge->m_shape.setSize(sf::Vector2f(1.0f * CHOICE_GAUGE_SIZE_X * m_gauge_value / m_gauge_value_max, CHOICE_GAUGE_SIZE_Y));
		m_gauge->m_shape.setOrigin(sf::Vector2f(m_gauge->m_shape.getSize().x * 0.5, CHOICE_GAUGE_SIZE_Y * 0.5f));
		m_gauge->m_shape.setFillColor(sf::Color::Green);
		m_gauge->m_shape.setOutlineThickness(4.f);
		m_gauge->m_shape.setOutlineColor(sf::Color::Black);
		m_gauge->m_shape.setPosition(sf::Vector2f(m_gauge->m_shape_container.getPosition().x - CHOICE_GAUGE_SIZE_X * 0.5 + m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape_container.getPosition().y));
	
		if (m_skill >= 0 && m_skill < NB_CREW_SKILLS)
		{
			m_gauge->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_gauge->m_text.setCharacterSize(20);
			m_gauge->m_text.setColor(sf::Color::Black);
			m_gauge->m_text.setString((*CurrentGame).m_dico_crew_skills[m_skill]);
			m_gauge->m_text.setPosition(0.f - CHOICE_PANEL_SIZE_X * 0.5 + CHOICE_VIDEO_SIZE_X + 20, m_gauge->m_shape_container.getPosition().y - CHOICE_GAUGE_SIZE_Y * 0.5 - m_gauge->m_text.getCharacterSize() - 14);
			m_gauge_string = m_gauge->m_text.getString();
		}
	}

	//costs displayed
	for (int i = 0; i < NB_RESOURCES_TYPES; i++)
	{
		if (m_cost[i] != 0)
		{
			GameEntity* cost_displayed = new GameEntity(UI_None);
			float pos_x = m_picture->m_text.getPosition().x + RESOURCES_ICON_SIZE * 0.5 + (m_costs_displayed.size() * CHOICES_COSTS_OFFSET_X);
			sf::Texture* texture_cost = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_dico_resources_textures[i], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
			cost_displayed->setAnimation(texture_cost, 1, 1);
			cost_displayed->setPosition(sf::Vector2f(pos_x, -RESOURCES_ICON_SIZE * 0.5));

			cost_displayed->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_STOCK_SIZE_Y));
			cost_displayed->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5f));
			cost_displayed->m_shape.setFillColor(sf::Color::Black);
			cost_displayed->m_shape.setPosition(cost_displayed->getPosition());

			ostringstream ss_resource;
			ss_resource << m_cost[i];
			cost_displayed->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
			cost_displayed->m_text.setCharacterSize(20);
			cost_displayed->m_text.setColor(sf::Color::Black);
			cost_displayed->m_text.setString(ss_resource.str());
			cost_displayed->m_text.setPosition(sf::Vector2f(cost_displayed->m_shape.getPosition().x + RESOURCES_ICON_SIZE * 0.5f + 4, cost_displayed->m_shape.getPosition().y - cost_displayed->m_text.getCharacterSize() * 0.65));
			
			m_costs_displayed.push_back(cost_displayed);
		}
	}
}

void Choice::SetPosition(sf::Vector2f position)
{
	sf::Vector2f offset = position - m_position;

	m_panel->m_shape_container.setPosition(m_panel->m_shape_container.getPosition() + offset);
	m_panel->m_text.setPosition(m_panel->m_text.getPosition() + offset);
	m_picture->m_shape_container.setPosition(m_picture->m_shape_container.getPosition() + offset);
	m_picture->m_text.setPosition(m_picture->m_text.getPosition() + offset);
	m_picture->setPosition(m_picture->getPosition() + offset);
	m_title.setPosition(m_title.getPosition() + offset);
	m_body.setPosition(m_body.getPosition() + offset);
	m_gauge->m_shape_container.setPosition(m_gauge->m_shape_container.getPosition() + offset);
	m_gauge->m_shape.setPosition(m_gauge->m_shape.getPosition() + offset);
	m_gauge->m_text.setPosition(m_gauge->m_text.getPosition() + offset);

	for (vector<GameEntity*>::iterator it = m_costs_displayed.begin(); it != m_costs_displayed.end(); it++)
	{
		(*it)->setPosition((*it)->getPosition() + offset);
		(*it)->m_shape.setPosition((*it)->m_shape.getPosition() + offset);
		(*it)->m_text.setPosition((*it)->m_text.getPosition() + offset);
	}
}

bool Choice::Update(int gauge_value)
{
	bool hovering_on = false;
	if (m_panel->IsHoveredByMouse() == true)
	{
		m_picture->setAnimationLine(0);
		hovering_on = true;
	}
	else if (m_picture->m_currentAnimationIndex == 0)
	{
		m_picture->setAnimationLine(1);
	}

	//text
	ostringstream ss;
	ss << m_gauge_string << " " << m_gauge_value << "%";
	m_gauge->m_text.setString(ss.str());

	if (m_gauge_value_max > 0)
	{
		m_gauge_value = gauge_value;
		m_gauge->m_shape.setSize(sf::Vector2f(1.0f * CHOICE_GAUGE_SIZE_X * gauge_value / m_gauge_value_max, CHOICE_GAUGE_SIZE_Y));
		m_gauge->m_shape.setOrigin(sf::Vector2f(m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape.getSize().y * 0.5));
		m_gauge->m_shape.setPosition(sf::Vector2f(m_gauge->m_shape_container.getPosition().x - CHOICE_GAUGE_SIZE_X * 0.5 + m_gauge->m_shape.getSize().x * 0.5, m_gauge->m_shape_container.getPosition().y));
	}

	return hovering_on;
}

void Choice::Draw(sf::RenderTexture& screen)
{
	screen.draw(m_panel->m_shape_container);
	screen.draw(m_panel->m_text);
	screen.draw(m_picture->m_shape_container);
	screen.draw(m_picture->m_text);
	m_picture->Draw(screen);
	screen.draw(m_title);
	screen.draw(m_body);
	screen.draw(m_gauge->m_shape_container);
	if (m_gauge_value > 0)
	{
		screen.draw(m_gauge->m_shape);
	}
	screen.draw(m_gauge->m_text);

	for (vector<GameEntity*>::iterator it = m_costs_displayed.begin(); it != m_costs_displayed.end(); it++)
	{
		(*it)->Draw(screen);
	}
}

int Choice::RandomizeRewardID()
{
	int rewardID;
	float random = RandomizeFloatBetweenValues(0, 100);
	int proba_cumulated = 0;
	for (int i = 0; i < NB_CHOICE_REWARDS_MAX; i++)
	{
		if (random < proba_cumulated + m_rewardsID[i].second || i == NB_CHOICE_REWARDS_MAX - 1)
		{
			return m_rewardsID[i].first;
		}
		else
		{
			proba_cumulated += m_rewardsID[i].second;
		}
	}

	return -1;
}