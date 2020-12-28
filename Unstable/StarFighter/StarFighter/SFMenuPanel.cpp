#include "SFMenuPanel.h"

extern Game* CurrentGame;

SFMenuPanel::SFMenuPanel(sf::Vector2f size, SFPanelTypes panel_type, size_t options, Ship* playership) : SFPanel(size, panel_type)
{
	m_options = options;
	m_playership = playership;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
}

SFMenuPanel::~SFMenuPanel()
{
	
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

		if (m_actions)
		{
			m_actions->Draw(screen);
		}
	}
}

//------------------ONE ACTION MENU------------
SFOneActionPanel::SFOneActionPanel(sf::Vector2f size, Ship* playership) : SFMenuPanel(size, SFPanel_Action, 1, playership)
{
	if (m_playership && m_playership->m_targetPortal)
	{
		//direction
		m_direction = m_playership->m_targetPortal->m_direction;

		//texts strings
		m_title_text.setString(m_playership->m_targetPortal->m_display_name);

		//options texts
		m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
		m_actions->SetString("Enter", ActionButton_A);

		//size and position of panel
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		sf::Vector2f position;

		if (m_direction == DIRECTION_UP)
			position = sf::Vector2f(SCENE_SIZE_X / 2, PORTAL_HEIGHT + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2);
		else if (m_direction == DIRECTION_DOWN)
			position = sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y - (PORTAL_HEIGHT + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));
		else if (m_direction == DIRECTION_LEFT)
			position = sf::Vector2f(getSize().x / 2 + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2 - (PORTAL_HEIGHT / 2 + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));
		else if (m_direction == DIRECTION_RIGHT)
			position = sf::Vector2f(SCENE_SIZE_X - (getSize().x / 2 + INTERACTION_PANEL_OFFSET_Y), SCENE_SIZE_Y / 2 - (PORTAL_HEIGHT / 2 + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));

		setPosition(position.x, position.y);

		//positioning of panel's content
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		//options texts
		text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
		m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}
}

//-------------------PORTAL MENU----------------
SFPortalPanel::SFPortalPanel(sf::Vector2f size, Ship* playership) : SFMenuPanel(size, SFPanel_Portal, NB_HAZARD_LEVELS, playership)
{
	if (m_playership && m_playership->m_targetPortal)
	{	
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

		//direction
		m_direction = m_playership->m_targetPortal->m_direction;

		//texts strings
		ostringstream ss_title;
		ss_title << m_playership->m_targetPortal->m_display_name << " (Level " << m_playership->m_targetPortal->m_level;// stoi((*CurrentGame).m_generalScenesConfig[m_playership->m_targetPortal->m_destination_name][SCENE_LEVEL]) << ")";
		m_title_text.setString(ss_title.str());

		//options texts
		m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
		m_actions->SetString("Enter", ActionButton_A);
		m_actions->SetString("Hazard down", ActionButton_Y);
		m_actions->SetString("Hazard up", ActionButton_X);

		m_actions_with_selection = new SFActionBoxWithSelection();
		/*for (int i = 0; i < NB_HAZARD_LEVELS; i++)
		{
			stringstream ss;
			ss << "Hazard " << i + 1;
			if (m_playership->m_targetPortal->m_max_unlocked_hazard_level == i)
			{
				//ss << "-> Score 100.0% to unlock next hazard level";
			}
			m_actions_with_selection->AddOption(ss.str(), (*CurrentGame).m_font[Font_Arial]);
			
			//text color
			if (i > m_playership->m_targetPortal->m_max_unlocked_hazard_level)
			{
				m_actions_with_selection->m_texts[i].setColor(sf::Color(80, 80, 80, 255));//greyed
			}
			else
			{
				if (m_playership->m_targetPortal->m_level + i < m_playership->m_level)
				{
					m_actions_with_selection->m_texts[i].setColor(sf::Color(40, 255, 40, 255));//green
				}
				else if (m_playership->m_targetPortal->m_level + i == m_playership->m_level +1)
				{
					m_actions_with_selection->m_texts[i].setColor(sf::Color(240, 150, 60, 255));//orange
				}
				else if (m_playership->m_targetPortal->m_level + i > m_playership->m_level + 1)
				{
					m_actions_with_selection->m_texts[i].setColor(sf::Color(255, 40, 40, 255));//red
				}
				else
				{
					m_actions_with_selection->m_texts[i].setColor(sf::Color::White);
				}
			}
		}
		*/

		//size and position of panel
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		sf::Vector2f position;
		if (m_direction == DIRECTION_UP)
			position = sf::Vector2f(SCENE_SIZE_X / 2, PORTAL_HEIGHT + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2);
		else if (m_direction == DIRECTION_DOWN)
			position = sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y - (PORTAL_HEIGHT + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));
		else if (m_direction == DIRECTION_LEFT)
			position = sf::Vector2f(getSize().x / 2 + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2 - (PORTAL_HEIGHT / 2 + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));
		else if (m_direction == DIRECTION_RIGHT)
			position = sf::Vector2f(SCENE_SIZE_X - (getSize().x / 2 + INTERACTION_PANEL_OFFSET_Y), SCENE_SIZE_Y / 2 - (PORTAL_HEIGHT / 2 + INTERACTION_PANEL_OFFSET_Y + getSize().y / 2));
		
		setPosition(position.x, position.y);

		//positioning of panel's content
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
		
		//options texts
		text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height/2;
		m_actions_with_selection->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

		//hint texts
		m_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES + 130, getPosition().y - getSize().y / 2 + text_height + 7));

		//actions texts
		text_height += 8*INTERACTION_INTERBLOCK;
		m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

		//default selected index
		//m_selected_option_index = (m_playership->m_last_hazard_level_played <= m_playership->m_targetPortal->m_max_unlocked_hazard_level ? m_playership->m_last_hazard_level_played : m_playership->m_targetPortal->m_max_unlocked_hazard_level);
		//m_actions_with_selection->SetSelectedAction(m_selected_option_index);
	}
}

void SFPortalPanel::SetSelectedOptionIndex(int index)
{
	m_selected_option_index = index;

	//update feedback of selected index
	m_actions_with_selection->SetSelectedAction(m_selected_option_index);
}

int SFPortalPanel::GetSelectedOptionIndex()
{
	return m_selected_option_index;
}

void SFPortalPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//updating greyed out options
	if (m_visible)
	{
		if (m_playership && m_playership->m_targetPortal)
		{
			if (m_selected_option_index == 0)
				m_actions->m_texts[ActionButton_Y].setColor(sf::Color(80, 80, 80, 255));//greyed
			else
				m_actions->m_texts[ActionButton_Y].setColor(sf::Color(255, 255, 255, 255));//white

			//if (m_selected_option_index == m_playership->m_targetPortal->m_max_unlocked_hazard_level)
			//	m_actions->m_texts[ActionButton_X].setColor(sf::Color(80, 80, 80, 255));//greyed
			//else
			//	m_actions->m_texts[ActionButton_X].setColor(sf::Color(255, 255, 255, 255));//white
		}

		//hint text
		//if (m_selected_option_index > 0)
		//{
		//	ostringstream ss;
		//	ss << "Bonus:\n+" << (int)(HazardLevelsBeastBonus[m_selected_option_index]*100/(2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES)) << "% quality drop\n\n" << "Malus:\n+" << (enemySpeedModifierTable[m_selected_option_index] - 1) * 100 << "% enemy presence\n" << "+" << (enemySpeedModifierTable[m_selected_option_index] - 1) * 100 << "% enemy speed and bullets speed";
		//	m_text.setString(ss.str());
		//}
		//else
		//	m_text.setString("");
	}
}

void SFPortalPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);
		
		if (m_actions_with_selection)
			m_actions_with_selection->Draw(screen);
		

		if (m_actions)
			m_actions->Draw(screen);
	}
}

//-------------------SHOP MENU----------------
SFShopPanel::SFShopPanel(sf::Vector2f size, Ship* playership) : SFMenuPanel(size, SFPanel_Shop, NBVAL_ShopOptions, playership)
{
	if (m_playership && m_playership->m_targetShop)
	{
		//texts strings
		m_title_text.setString(m_playership->m_targetShop->m_display_name);

		m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
		//m_actions->SetString("Buy/Sell", ActionButton_A);
		m_actions->SetString("Buy upgrades", ActionButton_A);
		m_actions->SetString("Stellar map", ActionButton_X);

		//size and position
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

		//positioning panel content
		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		//options texts
		text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
		m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}
}