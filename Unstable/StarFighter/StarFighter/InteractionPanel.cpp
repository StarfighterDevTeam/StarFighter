#include "InteractionPanel.h"

InteractionPanel::InteractionPanel()
{
	m_selected_index = 0;
	m_currentInteractionType = No_Interaction;
	m_currentShopMenu = ShopMainMenu;

	m_cursor = new GameObject(sf::Vector2f(HUD_LEFT_MARGIN + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_fakeShopGrid = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), true);
	m_shopGrid = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false);

	m_arrow = new GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
		sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));

	sf::Color _darkblue = sf::Color::Color(6, 87, 94, 255);//dark blue-green
	sf::Color _white = sf::Color::Color(255, 255, 255, 255);//semi-transparent white
	sf::Color _grey = sf::Color::Color(80, 80, 80, 255);//semi-transparent dark grey
	sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow

	m_panel.setSize(sf::Vector2f(INTERACTION_PANEL_WIDTH, INTERACTION_PANEL_HEIGHT));
	m_panel.setOrigin(sf::Vector2f(INTERACTION_PANEL_WIDTH / 2, INTERACTION_PANEL_HEIGHT / 2));
	m_panel.setFillColor(sf::Color(10, 10, 10, 230));//dark grey
	m_panel.setOutlineThickness(2);
	m_panel.setOutlineColor(_darkblue);//dark blue-green

	try
	{
		m_font = new sf::Font();
		if (!m_font->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}

		ostringstream ss_destination;
		ss_destination << "???Unknown???";
		m_textDestination.setFont(*m_font);
		m_textDestination.setCharacterSize(20);
		m_textDestination.setColor(_yellow);
		m_textDestination.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP);
		m_textDestination.setString(ss_destination.str());

		for (int i = 0; i < NB_HAZARD_LEVELS; i++)
		{
			ostringstream ss;
			ss << "\n\nHazard " << i + 1;

			m_text[i].setFont(*m_font);
			m_text[i].setCharacterSize(18);
			m_text[i].setColor(_white);
			m_text[i].setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i));
			m_text[i].setString(ss.str());
		}

		for (int i = 0; i < NBVAL_ShopOptions; i++)
		{
			m_textShopOptions[i].setFont(*m_font);
			m_textShopOptions[i].setCharacterSize(18);
			m_textShopOptions[i].setColor(_white);
			m_textShopOptions[i].setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i));
		}
		m_textShopOptions[ShopHeal].setString("\n\nHeal");
		m_textShopOptions[ShopBuy].setString("\n\nBuy");
		m_textShopOptions[StellarMap].setString("\n\nStellar map");

		ostringstream ss_helpNavigation;
		ss_helpNavigation << "\n\n\nFire: select\nBrake: down\nHyperspeed: up";
		m_textHelpNavigation.setFont(*m_font);
		m_textHelpNavigation.setCharacterSize(18);
		m_textHelpNavigation.setColor(_yellow);
		m_textHelpNavigation.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS));
		m_textHelpNavigation.setString(ss_helpNavigation.str());

		ostringstream ss_helpBuy;
		ss_helpBuy << "\n\n\nFire: buy\nSlowmotion: exit\n";
		m_textHelpBuy.setFont(*m_font);
		m_textHelpBuy.setCharacterSize(18);
		m_textHelpBuy.setColor(_yellow);
		m_textHelpBuy.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS));
		m_textHelpBuy.setString(ss_helpBuy.str());
	}

	catch (const std::exception & ex)
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR, ex.what());
	}
}

void InteractionPanel::UpdateShopInteraction(sf::Time deltaTime)
{
	switch (m_currentShopMenu)
	{
		case ShopMainMenu:
		{
			sf::Vector2f l_size = sf::Vector2f(INTERACTION_PANEL_WIDTH, (2 * INTERACTION_PANEL_MARGIN_TOP) + (INTERACTION_INTERLINE * (NBVAL_ShopOptions + 3)) + m_textDestination.getCharacterSize() + (m_textHelpNavigation.getCharacterSize() * 3));
			m_panel.setSize(l_size);
			m_panel.setOrigin(l_size.x / 2, l_size.y / 2);

			sf::Vector2f position = sf::Vector2f((SCENE_SIZE_X / 2) + (PORTAL_WIDTH / 2) + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2);

			m_panel.setPosition(position.x, position.y);

			for (int i = 0; i < NBVAL_ShopOptions; i++)
			{
				m_textShopOptions[i].setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i) - (l_size.y / 2));
			}

			m_textDestination.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP - (l_size.y / 2));

			m_textHelpNavigation.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NBVAL_ShopOptions) - (l_size.y / 2));

			m_arrow->setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * m_selected_index) + (m_textDestination.getCharacterSize() / 2) + (m_text[0].getCharacterSize() * (4 + 1) / 2) - (l_size.y / 2));

			break;
		}

		case ShopBuyMenu:
		{
			sf::Vector2f l_size = GetShopBuyPanelSize();
			m_panel.setSize(l_size);
			m_panel.setOrigin(l_size.x / 2, l_size.y / 2);

			sf::Vector2f position = sf::Vector2f((SCENE_SIZE_X / 2) + (PORTAL_WIDTH / 2) + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2);

			m_panel.setPosition(position);

			m_fakeShopGrid.SetGridPosition(sf::Vector2f(position.x + INTERACTION_PANEL_MARGIN_SIDES - (l_size.x / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + m_textDestination.getCharacterSize() + INTERACTION_INTERLINE - (l_size.y / 2)));
			m_shopGrid.SetGridPosition(sf::Vector2f(position.x + INTERACTION_PANEL_MARGIN_SIDES - (l_size.x / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + m_textDestination.getCharacterSize() + INTERACTION_INTERLINE - (l_size.y / 2)));

			m_textDestination.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP - (l_size.y / 2));

			m_textHelpBuy.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (SHOP_GRID_NB_LINES * SHOP_GRID_SLOT_SIZE) - (l_size.y / 2));

			m_cursor->update(deltaTime);
			//panel constraints
			if (m_cursor->getPosition().x < m_panel.getPosition().x - (GetShopBuyPanelSize().x / 2))
				m_cursor->setPosition(m_panel.getPosition().x - (GetShopBuyPanelSize().x / 2), m_cursor->getPosition().y);
			if (m_cursor->getPosition().x > m_panel.getPosition().x + (GetShopBuyPanelSize().x / 2))
				m_cursor->setPosition(m_panel.getPosition().x + (GetShopBuyPanelSize().x / 2), m_cursor->getPosition().y);
			if (m_cursor->getPosition().y < m_panel.getPosition().y - (GetShopBuyPanelSize().y / 2))
				m_cursor->setPosition(m_cursor->getPosition().x, m_panel.getPosition().y - (GetShopBuyPanelSize().y / 2));
			if (m_cursor->getPosition().y > m_panel.getPosition().y + (GetShopBuyPanelSize().y / 2))
				m_cursor->setPosition(m_cursor->getPosition().x, m_panel.getPosition().y + (GetShopBuyPanelSize().y / 2));

			break;
		}

		case StellarMap:
		{
			sf::Vector2f l_size = GetShopBuyPanelSize();
			m_panel.setSize(l_size);
			m_panel.setOrigin(l_size.x / 2, l_size.y / 2);

			sf::Vector2f position = sf::Vector2f((SCENE_SIZE_X / 2) + (PORTAL_WIDTH / 2) + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2);

			m_panel.setPosition(position);

			m_fakeShopGrid.SetGridPosition(sf::Vector2f(position.x + INTERACTION_PANEL_MARGIN_SIDES - (l_size.x / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + m_textDestination.getCharacterSize() + INTERACTION_INTERLINE - (l_size.y / 2)));
			m_shopGrid.SetGridPosition(sf::Vector2f(position.x + INTERACTION_PANEL_MARGIN_SIDES - (l_size.x / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + m_textDestination.getCharacterSize() + INTERACTION_INTERLINE - (l_size.y / 2)));

			m_textDestination.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP - (l_size.y / 2));

			m_textHelpBuy.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (SHOP_GRID_NB_LINES * SHOP_GRID_SLOT_SIZE) - (l_size.y / 2));

			m_cursor->update(deltaTime);
			//panel constraints
			if (m_cursor->getPosition().x < m_panel.getPosition().x - (GetShopBuyPanelSize().x / 2))
				m_cursor->setPosition(m_panel.getPosition().x - (GetShopBuyPanelSize().x / 2), m_cursor->getPosition().y);
			if (m_cursor->getPosition().x > m_panel.getPosition().x + (GetShopBuyPanelSize().x / 2))
				m_cursor->setPosition(m_panel.getPosition().x + (GetShopBuyPanelSize().x / 2), m_cursor->getPosition().y);
			if (m_cursor->getPosition().y < m_panel.getPosition().y - (GetShopBuyPanelSize().y / 2))
				m_cursor->setPosition(m_cursor->getPosition().x, m_panel.getPosition().y - (GetShopBuyPanelSize().y / 2));
			if (m_cursor->getPosition().y > m_panel.getPosition().y + (GetShopBuyPanelSize().y / 2))
				m_cursor->setPosition(m_cursor->getPosition().x, m_panel.getPosition().y + (GetShopBuyPanelSize().y / 2));

			break;
		}
	}
}

sf::Vector2f InteractionPanel::GetShopBuyPanelSize()
{
	return sf::Vector2f((2 * INTERACTION_PANEL_MARGIN_SIDES) + (SHOP_GRID_NB_ROWS * SHOP_GRID_SLOT_SIZE), (2 * INTERACTION_PANEL_MARGIN_TOP) + (SHOP_GRID_NB_LINES * SHOP_GRID_SLOT_SIZE) + m_textDestination.getCharacterSize() + (m_textHelpNavigation.getCharacterSize() * 5));
}

void InteractionPanel::UpdatePortalInteraction(int max_unlocked_hazard_level)
{
	sf::Vector2f l_size = sf::Vector2f(INTERACTION_PANEL_WIDTH, (2 * INTERACTION_PANEL_MARGIN_TOP) + (INTERACTION_INTERLINE * (NB_HAZARD_LEVELS + 3)) + m_textDestination.getCharacterSize() + (m_textHelpNavigation.getCharacterSize() * 3));
	m_panel.setSize(l_size);
	m_panel.setOrigin(l_size.x / 2, l_size.y / 2);

	sf::Vector2f position = sf::Vector2f((SCENE_SIZE_X / 2) + (PORTAL_WIDTH / 2) + INTERACTION_PANEL_OFFSET_Y, SCENE_SIZE_Y / 2);
	if (m_direction != NO_DIRECTION)
	{
		sf::Vector2f l_sizeNormalized = GameObject::getSize_for_Direction(m_direction, l_size);
		position = GameObject::getPosition_for_Direction(m_direction, sf::Vector2f(SCENE_SIZE_X / 2, PORTAL_HEIGHT + l_sizeNormalized.y + INTERACTION_PANEL_OFFSET_Y));
	}

	m_panel.setPosition(position.x, position.y);

	for (int i = 0; i < NB_HAZARD_LEVELS; i++)
	{
		m_text[i].setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * i) - (l_size.y / 2));
	}

	m_textDestination.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP - (l_size.y / 2));
	
	m_textHelpNavigation.setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS) - (l_size.y / 2));

	m_arrow->setPosition(position.x + INTERACTION_PANEL_MARGIN_SIDES - (INTERACTION_PANEL_WIDTH / 2), position.y + INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * m_selected_index) + (m_textDestination.getCharacterSize() / 2) + (m_text[0].getCharacterSize() * (4 + 1) / 2) - (l_size.y / 2));

	for (int i = 0; i < NB_HAZARD_LEVELS; i++)
	{
		if (i > max_unlocked_hazard_level)
		{
			m_text[i].setColor(sf::Color(80, 80, 80, 255));//greyed
		}
		else
		{
			m_text[i].setColor(sf::Color(255, 255, 255, 255));//white
		}
	}
}

void InteractionPanel::Update(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime)
{
	m_currentInteractionType = interaction;

	switch (interaction)
	{
	case PortalInteraction:
		if (max_unlocked_hazard_level > -1 && max_unlocked_hazard_level < NB_HAZARD_LEVELS)
		{
			UpdatePortalInteraction(max_unlocked_hazard_level);
		}
		break;
	case ShopInteraction:
		UpdateShopInteraction(deltaTime);
		break;
	}
}

void InteractionPanel::InitCursorOnGrid()
{
	m_cursor->setPosition(sf::Vector2f(m_panel.getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - (GetShopBuyPanelSize().x / 2) + (SHOP_GRID_SLOT_SIZE / 2), m_panel.getPosition().y + INTERACTION_PANEL_MARGIN_TOP + m_textDestination.getCharacterSize() + INTERACTION_INTERLINE - (GetShopBuyPanelSize().y / 2) + (SHOP_GRID_SLOT_SIZE / 2)));
	//m_cursor->setPosition(sf::Vector2f(m_panel.getPosition().x - (GetShopBuyPanelSize().x / 2) + m_fakeShopGrid.grid[0][0]->getPosition().x, m_panel.getPosition().y - (GetShopBuyPanelSize().y / 2) + m_fakeShopGrid.grid[0][0]->getPosition().y + INTERACTION_PANEL_MARGIN_TOP));
}

void InteractionPanel::Draw(sf::RenderTexture& screen)
{
	if (m_currentInteractionType != No_Interaction)
	{
		screen.draw(m_panel);
		screen.draw(m_textDestination);

		if (m_currentShopMenu == ShopMainMenu)
		{
			screen.draw(*m_arrow);

			if (m_currentInteractionType == ShopInteraction)
			{
				screen.draw(m_textHelpNavigation);
				for (int i = 0; i < NBVAL_ShopOptions; i++)
				{
					screen.draw(m_textShopOptions[i]);
				}
			}
		}
		else if (m_currentShopMenu == ShopBuyMenu)
		{
			m_fakeShopGrid.Draw(screen);
			m_shopGrid.Draw(screen);
			screen.draw(m_textHelpBuy);
			screen.draw(*m_cursor);
		}

		if (m_currentInteractionType == PortalInteraction)
		{
			for (int i = 0; i < NB_HAZARD_LEVELS; i++)
			{
				screen.draw(m_text[i]);
				screen.draw(m_textHelpNavigation);
			}
		}
	}
}