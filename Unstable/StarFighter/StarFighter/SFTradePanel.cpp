#include "SFTradePanel.h"

extern Game* CurrentGame;

//TRADE PANEL
SFTradePanel::SFTradePanel(sf::Vector2f size, Ship* playerShip) : SFInventoryPanel(size, playerShip, SFPanel_Trade)
{
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_item_stats_panel_compare = NULL;

	m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	m_actions->SetString("Quit", ActionButton_B);

	m_title_text2.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_title_text2.setCharacterSize(m_title_text.getCharacterSize());
	m_title_text2.setColor(m_title_text.getColor());
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts
	if (playerShip)
	{
		//player name and content
		m_title_text2.setString(m_playerShip->m_display_name);

		//shop name and items
		if (m_playerShip->m_targetShop != NULL)
			m_title_text.setString(m_playerShip->m_targetShop->m_display_name);
	}

	//size and position
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

	//positioning panel content
	float text_height = 0;

	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	m_title_text2.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	text_height += INTERACTION_INTERBLOCK;
	text_height += INTERACTION_INTERBLOCK;

	sf::Vector2f position0 = sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	sf::Vector2f position2 = sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	float text_height2 = text_height += INTERACTION_INTERBLOCK + 2 * GRID_SLOT_SIZE;

	sf::Vector2f position1 = sf::Vector2f(position0.x, position0.y + INTERACTION_INTERBLOCK + GRID_SLOT_SIZE);
	
	//quit button
	text_height += SHOP_GRID_NB_LINES * GRID_SLOT_SIZE;
	text_height -= 4 * INTERACTION_INTERBLOCK;
	m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//build separator between shop items and player items
	m_separator.setSize(sf::Vector2f(1, size.y));
	m_separator.setOrigin(m_separator.getSize().x / 2, m_separator.getSize().y/2);
	m_separator.setPosition(getPosition());
	m_separator.setFillColor(getOutlineColor());

	//Create grids
	CreateGrids(this, position0, position1, position2);
}

void SFTradePanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFInventoryPanel::Draw(screen);

		screen.draw(m_separator);
		screen.draw(m_title_text);
		screen.draw(m_title_text2);

		if (m_item_stats_panel)
		{
			m_item_stats_panel->Draw(screen);
		}
		if (m_item_stats_panel_compare)
		{
			m_item_stats_panel_compare->Draw(screen);
		}
	}
}
