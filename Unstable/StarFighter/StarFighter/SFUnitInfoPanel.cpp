#include "SFUnitInfoPanel.h"

extern Game* CurrentGame;

SFUnitInfoPanel::SFUnitInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts strings
	ostringstream ss_title;
	StockEntity* entity = NULL;
	if (playerShip->m_hovered_object)
	{
		entity = (StockEntity*)playerShip->m_hovered_object;
		ss_title << entity->m_display_name;
	}
	else if (playerShip->m_selected_object)
	{
		entity = (StockEntity*)playerShip->m_selected_object;
		ss_title << entity->m_display_name;
	}
	m_unit = entity;

	if (!m_unit)
	{
		return;
	}
	
	m_title_text.setString(ss_title.str());

	//text
	ostringstream ss_text;
	ostringstream ss_ore;
	string contains_ore;
	ostringstream ss_fuel;
	string contains_fuel;

	for (map<string, size_t>::iterator i = entity->m_ores_stocked.begin(); i != entity->m_ores_stocked.end(); i++)
	{
		ss_ore << "\n" << i->first << ": " << i->second;
	}
	contains_ore = ss_ore.str();
	if (!contains_ore.empty())
	{
		ss_text << "Ore: " << entity->m_stock << " / " << entity->GetStockMax() << contains_ore;
	}

	for (map<string, size_t>::iterator i = entity->m_fuel_tanks.begin(); i != entity->m_fuel_tanks.end(); i++)
	{
		ss_fuel << "\n" << i->first << ": " << i->second;
	}
	contains_fuel = ss_fuel.str();
	if (!contains_fuel.empty())
	{
		if (!contains_ore.empty())
		{
			ss_text << "\n\n";
		}
		ss_text << "Fuel: " << entity->m_fuel << " / " << entity->GetFuelMax() << contains_fuel;
	}

	m_text.setString(ss_text.str());


	//options texts
	//m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	//m_actions->SetString("Enter", ActionButton_A);
	//m_actions->SetString("Hazard down", ActionButton_Y);
	//m_actions->SetString("Hazard up", ActionButton_X);
	//
	//m_actions_with_selection = new SFActionBoxWithSelection();
	//for (int i = 0; i < 4; i++)
	//{
	//	stringstream ss;
	//	ss << "Action " << i;
	//	m_actions_with_selection->AddOption(ss.str(), (*CurrentGame).m_font[Font_Arial]);
	//}

	sf::Vector2f position = sf::Vector2f(size.x / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + playerShip->getPosition().x, size.y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + playerShip->getPosition().y);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//options texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
}

SFUnitInfoPanel::~SFUnitInfoPanel()
{
	
}

void SFUnitInfoPanel::Update(sf::Time deltaTime)
{
	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//dynamic size
	text_height += INTERACTION_INTERBLOCK + m_text.getGlobalBounds().height + INTERACTION_PANEL_MARGIN_TOP;
	setSize(sf::Vector2f(getSize().x, text_height));
	setOrigin(sf::Vector2f(getOrigin().x, text_height/2));
	sf::Vector2f position = sf::Vector2f(getPosition().x, getSize().y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + m_playerShip->getPosition().y);
	setPosition(position.x, position.y);

	//options texts
	//text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
}

void SFUnitInfoPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);

		//if (m_actions)
		//{
		//	m_actions->Draw(screen);
		//}
	}
}

GameObject* SFUnitInfoPanel::GetUnit()
{
	return m_unit;
}