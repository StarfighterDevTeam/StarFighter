#include "SFMenuPanel.h"

extern Game* CurrentGame;

SFMenuPanel::SFMenuPanel(sf::Vector2f size, SFPanelTypes panel_type, size_t options, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_options = options;
	m_options_text = new sf::Text[options];
	m_selected_option_index = 0;
	m_playerShip = playerShip;
	m_arrow = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
		sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//options texts
	for (size_t i = 0; i < options; i++)
	{
		m_options_text[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_options_text[i].setCharacterSize(18);
	}
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
}

SFMenuPanel::~SFMenuPanel()
{
	delete[] m_options_text;
}

void SFMenuPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//see override function in SFPortalPanel and SFShopPanel
}

void SFMenuPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		for (size_t i = 0; i < m_options; i++)
		{
			screen.draw(m_options_text[i]);
		}
		screen.draw(m_actions_text);
		screen.draw(m_arrow);
	}
}

void SFMenuPanel::SetSelectedOptionIndex(int index)
{
	m_selected_option_index = index;
}

int SFMenuPanel::GetSelectedOptionIndex()
{
	return m_selected_option_index;
}

//-------------------PORTAL MENU----------------
SFPortalPanel::SFPortalPanel(sf::Vector2f size, Ship* playerShip) : SFMenuPanel(size, SFPanel_Portal, NB_HAZARD_LEVELS, playerShip)
{
	if (m_playerShip && m_playerShip->m_targetPortal)
	{	
		//direction
		m_direction = m_playerShip->m_targetPortal->m_direction;

		//texts strings
		m_title_text.setString(m_playerShip->m_targetPortal->m_display_name);

		for (size_t i = 0; i < m_options; i++)
		{
			stringstream ss;
			ss << "Hazard " << i + 1;
			m_options_text[i].setString(ss.str());
		}

		ostringstream ss_helpNavigation;
		ss_helpNavigation << "\n\n\nFire: select\nBrake: down\nHyperspeed: up";
		m_actions_text.setString(ss_helpNavigation.str());

		//size and position of panel
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		sf::Vector2f position = sf::Vector2f((SCENE_SIZE_X / 2) + (PORTAL_WIDTH / 2) + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2);
		if (m_direction != NO_DIRECTION)
		{
			sf::Vector2f l_sizeNormalized = GameObject::getSize_for_Direction(m_direction, size);
			position = GameObject::getPosition_for_Direction(m_direction, sf::Vector2f(SCENE_SIZE_X / 2, PORTAL_HEIGHT + l_sizeNormalized.y/2 + INTERACTION_PANEL_OFFSET_Y));
		}
		setPosition(position.x, position.y);

		//positioning of panel's content
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);

		//options texts
		for (size_t i = 0; i < m_options; i++)
		{
			if (i == 0)
			{
				text_height += INTERACTION_INTERBLOCK;
			}
			else
			{
				text_height += INTERACTION_INTERLINE;
			}
			text_height += m_options_text[i].getGlobalBounds().height;
			m_options_text[i].setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);
		}

		//actions texts
		text_height += INTERACTION_INTERBLOCK;
		m_actions_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height - m_actions_text.getGlobalBounds().height/2);
		text_height += m_actions_text.getGlobalBounds().height;
		//default selected index
		//if (m_playerShip->m_previouslyCollidingWithInteractiveObject != PortalInteraction)
		//{
		m_selected_option_index = (m_playerShip->m_last_hazard_level_played <= m_playerShip->m_targetPortal->m_max_unlocked_hazard_level ? m_playerShip->m_last_hazard_level_played : m_playerShip->m_targetPortal->m_max_unlocked_hazard_level);
		//}
		}
}

void SFPortalPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//arrow
	m_arrow.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - (getSize().x / 2), m_options_text[m_selected_option_index].getPosition().y + m_options_text[m_selected_option_index].getGlobalBounds().height - 1);

	//updating greyed out options
	if (m_visible)
	{
		if (m_playerShip && m_playerShip->m_targetPortal)
		{
			for (int i = 0; i < NB_HAZARD_LEVELS; i++)
			{
				if (i > m_playerShip->m_targetPortal->m_max_unlocked_hazard_level)
				{
					m_options_text[i].setColor(sf::Color(80, 80, 80, 255));//greyed
				}
				else
				{
					m_options_text[i].setColor(sf::Color(255, 255, 255, 255));//white
				}
			}
		}
	}
}

//-------------------SHOP MENU----------------
SFShopPanel::SFShopPanel(sf::Vector2f size, Ship* playerShip) : SFMenuPanel(size, SFPanel_Shop, NBVAL_ShopOptions, playerShip)
{
	if (m_playerShip && m_playerShip->m_targetShop)
	{
		//texts strings
		m_title_text.setString(m_playerShip->m_targetShop->m_display_name);
		
		m_options_text[ShopHeal].setString("Heal");
		m_options_text[ShopBuy].setString("Buy/Sell");
		m_options_text[ShopStellarMap].setString("Stellar map");

		ostringstream ss_helpNavigation;
		ss_helpNavigation << "\n\n\nFire: select\nBrake: down\nHyperspeed: up";
		m_actions_text.setString(ss_helpNavigation.str());

		//size and position
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

		//positioning panel content
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);

		//options texts
		for (size_t i = 0; i < m_options; i++)
		{
			if (i == 0)
			{
				text_height += INTERACTION_INTERBLOCK;
			}
			else
			{
				text_height += INTERACTION_SHOP_INTERLINE;
			}
			text_height += m_options_text[i].getGlobalBounds().height;
			m_options_text[i].setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);
		}

		//actions texts
		text_height += INTERACTION_INTERBLOCK - m_actions_text.getGlobalBounds().height / 2;
		m_actions_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);
		text_height += m_actions_text.getGlobalBounds().height;
		//default selected index
		//if (m_playerShip->m_previouslyCollidingWithInteractiveObject != ShopInteraction)
		//{
		m_selected_option_index = 0;
			//}
	}
}

void SFShopPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//arrow
	m_arrow.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - (getSize().x / 2), m_options_text[m_selected_option_index].getPosition().y + m_options_text[0].getGlobalBounds().height - 1);
	//should be the selected option index's text global bounds, but for some reason it's inconsistent, based on what characters are used in the text, so let us use the same reference index for height
	//m_arrow->setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - (getSize().x / 2), m_options_text[m_selected_option_index].getPosition().y + m_options_text[m_selected_option_index].getGlobalBounds().height - 1);

	//updating greyed out options
	if (m_visible)
	{
		if (m_playerShip && m_playerShip->m_targetShop)
		{
			for (int i = 0; i < m_options; i++)
			{
				bool condition = true;//dynamic condition that can be used, such as checking player's money
				if (!condition)
				{
					m_options_text[i].setColor(sf::Color(80, 80, 80, 255));//greyed
				}
				else
				{
					m_options_text[i].setColor(sf::Color(255, 255, 255, 255));//white
				}
			}
		}
	}
}