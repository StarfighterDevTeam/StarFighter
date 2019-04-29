#include "RewardInterface.h"

extern Game* CurrentGame;

RewardInterface::RewardInterface()
{
	m_ship = NULL;
	m_reward = NULL;
	m_panel = NULL;
	m_ok_button = NULL;
}

RewardInterface::~RewardInterface()
{
	Destroy();
}

void RewardInterface::Destroy()
{
	m_ship = NULL;

	delete m_panel;
	m_panel = NULL;

	for (vector<GameEntity*>::iterator it = m_resources_displayed.begin(); it != m_resources_displayed.end(); it++)
	{
		delete (*it);
	}
	m_resources_displayed.clear();

	delete m_reward;
	m_reward = NULL;

	delete m_ok_button;
	m_ok_button = NULL;

	m_text_DMS_location.setString("");

	m_crew_killed.clear();
	m_crew_recruited.clear();
}

void RewardInterface::Init(Ship* ship, Reward* reward)
{
	m_ship = ship;
	m_reward = reward;

	//background panel
	m_panel = new GameEntity(UI_None);
	m_panel->m_shape_container.setSize(sf::Vector2f(REWARD_PANEL_SIZE_X, REWARD_PANEL_SIZE_Y));
	m_panel->m_shape_container.setOrigin(sf::Vector2f(REWARD_PANEL_SIZE_X * 0.5f, REWARD_PANEL_SIZE_Y * 0.5));
	m_panel->m_shape_container.setFillColor(sf::Color::Black);
	m_panel->m_shape_container.setOutlineThickness(2.f);
	m_panel->m_shape_container.setOutlineColor(sf::Color::Black);
	m_panel->m_shape_container.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - REWARD_PANEL_SIZE_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));
	m_panel->m_position = m_panel->m_shape_container.getPosition();
	m_panel->m_size = m_panel->m_shape_container.getSize();

	//text
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(18);
	m_text.setStyle(sf::Text::Bold);
	m_text.setColor(sf::Color::White);
	m_text.setString(reward->m_string);
	m_text.setPosition(sf::Vector2f(m_panel->m_position.x - m_panel->m_shape_container.getSize().x * 0.5 + 20, m_panel->m_position.y - m_panel->m_shape_container.getSize().y * 0.5 + m_text.getCharacterSize()));

	//display rewards
	int size = m_reward->m_resources.size();
	float centering_offset = size * CHOICES_REWARDS_OFFSET_X;
	centering_offset *= 0.25f;

	if (reward->m_DMS_location != NULL)
	{
		//secret location coordinates reveal
		ostringstream ss_DMS;
		DMS_Coord& dms = *reward->m_DMS_location;
		ss_DMS << dms.m_degree_y << "° " << dms.m_minute_y << "' " << (int)dms.m_second_y << " \"\N";
		ss_DMS << ", ";
		ss_DMS << dms.m_degree_x << "° " << dms.m_minute_x << "' " << (int)dms.m_second_x << " \"\E";

		m_text_DMS_location.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text_DMS_location.setCharacterSize(20);
		m_text_DMS_location.setColor(sf::Color::Green);
		m_text_DMS_location.setStyle(sf::Text::Bold);
		m_text_DMS_location.setString(ss_DMS.str());
		m_text_DMS_location.setPosition(sf::Vector2f(m_panel->m_position.x - m_text_DMS_location.getGlobalBounds().width * 0.5, m_panel->m_position.y - m_text_DMS_location.getGlobalBounds().height * 0.65 + 4));
	}

	for (int i = 0; i < size; i++)
	{
		//resources
		GameEntity* resources_displayed = new GameEntity(UI_None);
		float pos_x = m_panel->m_position.x - centering_offset + (CHOICES_REWARDS_OFFSET_X * i);// m_panel->m_shape_container.getSize().x * 0.5 + RESOURCES_ICON_SIZE * 0.5 + REWARD_INTERFACE_OFFSET_X + (CHOICES_REWARDS_OFFSET_X * i);

		sf::Texture* texture = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_dico_resources_textures[reward->m_resources[i].first], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
		resources_displayed->setAnimation(texture, 1, 1);
		resources_displayed->setPosition(sf::Vector2f(pos_x, m_panel->m_position.y + m_panel->m_shape_container.getSize().y * 0.5 - RESOURCES_ICON_SIZE * 0.5 - 60));

		resources_displayed->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_STOCK_SIZE_Y));
		resources_displayed->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_STOCK_SIZE_Y * 0.5f));
		resources_displayed->m_shape.setFillColor(sf::Color::Black);
		resources_displayed->m_shape.setPosition(resources_displayed->getPosition());

		ostringstream ss_resource;
		ss_resource << reward->m_resources[i].second;
		resources_displayed->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		resources_displayed->m_text.setCharacterSize(20);
		resources_displayed->m_text.setColor(sf::Color::White);
		resources_displayed->m_text.setString(ss_resource.str());
		resources_displayed->m_text.setPosition(sf::Vector2f(resources_displayed->m_shape.getPosition().x + RESOURCES_ICON_SIZE * 0.5f + 4, resources_displayed->m_shape.getPosition().y - resources_displayed->m_text.getCharacterSize() * 0.65));
		
		m_resources_displayed.push_back(resources_displayed);
	}

	//confirm button
	m_ok_button = new GameEntity(UI_None);
	m_ok_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_ok_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_ok_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_ok_button->m_shape_container.setOutlineThickness(2);
	m_ok_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_ok_button->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x, m_panel->m_position.y + m_panel->m_size.y * 0.5 - m_ok_button->m_shape_container.getSize().y * 0.5 - 20));
	
	m_ok_button->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_ok_button->m_text.setCharacterSize(18);
	m_ok_button->m_text.setStyle(sf::Text::Bold);
	m_ok_button->m_text.setColor(sf::Color::White);
	m_ok_button->m_text.setString("OK");
	m_ok_button->m_text.setPosition(sf::Vector2f(m_ok_button->m_shape_container.getPosition().x - m_ok_button->m_text.getGlobalBounds().width * 0.5, m_ok_button->m_shape_container.getPosition().y - m_ok_button->m_text.getCharacterSize() * 0.6));
}

bool RewardInterface::Update()
{
	//ok button
	if ((m_ok_button->IsHoveredByMouse() == true || (*CurrentGame).m_input_actions[Action_Entering] == Input_Tap) && (*CurrentGame).m_window_has_focus == true)
	{
		m_ok_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_CrewFidelity_Positive]);

		//action
		if ((*CurrentGame).m_mouse_click == Mouse_LeftClick || (*CurrentGame).m_input_actions[Action_Entering] == Input_Tap)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_ok_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
		return false;
	}
}

void RewardInterface::Draw(sf::RenderTexture& screen)
{
	m_panel->Draw(screen);

	for (vector<GameEntity*>::iterator it = m_resources_displayed.begin(); it != m_resources_displayed.end(); it++)
	{
		(*it)->Draw(screen);
	}

	screen.draw(m_text);
	screen.draw(m_text_DMS_location);

	m_ok_button->Draw(screen);
}