#include "RewardInterface.h"

extern Game* CurrentGame;

Reward::Reward()
{
	for (int i = 0; i < 3; i++)
	{
		m_rewards[i].second = 0;
	}
}

RewardInterface::RewardInterface()
{
	m_ship = NULL;
	m_reward = NULL;

	m_panel = NULL;
	for (int i = 0; i < 3; i++)
	{
		m_rewards[i] = NULL;
	}

	m_ok_button = NULL;
}

RewardInterface::~RewardInterface()
{
	Destroy();
}

void RewardInterface::Destroy()
{
	delete m_panel;
	m_panel = NULL;

	for (int i = 0; i < 3; i++)
	{
		delete m_rewards[i];
		m_rewards[i] = NULL;
	}

	delete m_reward;
	m_reward = NULL;

	m_ship = NULL;

	delete m_ok_button;
	m_ok_button = NULL;
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

	//text
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setCharacterSize(18);
	m_text.setStyle(sf::Text::Bold);
	m_text.setColor(sf::Color::White);
	m_text.setString(reward->m_string);
	m_text.setPosition(sf::Vector2f(m_panel->m_position.x - m_panel->m_shape_container.getSize().x * 0.5 + 20, m_panel->m_position.y - m_panel->m_shape_container.getSize().y * 0.5 + m_text.getCharacterSize()));

	//display rewards
	float centering_offset = 0;
	for (int i = 0; i < 3; i++)
	{
		if (reward->m_rewards[i].second == 0)
		{
			break;
		}
		else
		{
			centering_offset += CHOICES_REWARDS_OFFSET_X;
		}
	}
	centering_offset *= 0.25f;

	for (int i = 0; i < 3; i++)
	{
		if (reward->m_rewards[i].second == 0)
		{
			break;
		}

		m_rewards[i] = new GameEntity(UI_None);
		float pos_x = m_panel->m_position.x - centering_offset + (CHOICES_REWARDS_OFFSET_X * i);// m_panel->m_shape_container.getSize().x * 0.5 + RESOURCES_ICON_SIZE * 0.5 + REWARD_INTERFACE_OFFSET_X + (CHOICES_REWARDS_OFFSET_X * i);

		sf::Texture* texture = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_dico_resources_textures[reward->m_rewards[i].first], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
		m_rewards[i]->setAnimation(texture, 1, 1);
		m_rewards[i]->setPosition(sf::Vector2f(pos_x, m_panel->m_position.y + m_panel->m_shape_container.getSize().y * 0.5 - RESOURCES_ICON_SIZE * 0.5 - 60));

		m_rewards[i]->m_shape.setSize(sf::Vector2f(RESOURCES_ICON_SIZE, RESOURCES_INTERFACE_SIZE_Y));
		m_rewards[i]->m_shape.setOrigin(sf::Vector2f(RESOURCES_ICON_SIZE * 0.5f, RESOURCES_INTERFACE_SIZE_Y * 0.5f));
		m_rewards[i]->m_shape.setFillColor(sf::Color::Black);
		m_rewards[i]->m_shape.setPosition(m_rewards[i]->getPosition());

		ostringstream ss_resource;
		ss_resource << reward->m_rewards[i].second;
		m_rewards[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_rewards[i]->m_text.setCharacterSize(20);
		m_rewards[i]->m_text.setColor(sf::Color::White);
		m_rewards[i]->m_text.setString(ss_resource.str());
		m_rewards[i]->m_text.setPosition(sf::Vector2f(m_rewards[i]->m_shape.getPosition().x + RESOURCES_ICON_SIZE * 0.5f + 4, m_rewards[i]->m_shape.getPosition().y - m_rewards[i]->m_text.getCharacterSize() * 0.65));
	}

	//confirm button
	m_ok_button = new GameEntity(UI_None);
	m_ok_button->m_shape_container.setSize(sf::Vector2f(RESOURCES_BUTTON_SIZE_X, RESOURCES_BUTTON_SIZE_Y));
	m_ok_button->m_shape_container.setOrigin(sf::Vector2f(RESOURCES_BUTTON_SIZE_X * 0.5, RESOURCES_BUTTON_SIZE_Y * 0.5));
	m_ok_button->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_VeryDarkGrey_Background]);
	m_ok_button->m_shape_container.setOutlineThickness(2);
	m_ok_button->m_shape_container.setOutlineColor(sf::Color::White);
	m_ok_button->m_shape_container.setPosition(sf::Vector2f(m_panel->m_position.x, m_rewards[0]->m_shape.getPosition().y + 46));
	
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
			//give reward
			for (int i = 0; i < 3; i++)
			{
				if (m_reward->m_rewards[i].second != 0)
				{
					m_ship->AddResource(m_reward->m_rewards[i].first, m_reward->m_rewards[i].second);
				}
			}

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

	for (int i = 0; i < 3; i++)
	{
		if (m_reward->m_rewards[i].second != 0)
		{
			m_rewards[i]->Draw(screen);
		}
		else
		{
			break;
		}
	}

	screen.draw(m_text);

	m_ok_button->Draw(screen);
}