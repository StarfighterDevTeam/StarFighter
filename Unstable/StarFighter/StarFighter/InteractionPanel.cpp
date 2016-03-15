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

	m_itemStatsPanel.setSize(sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y));
	m_itemStatsPanel.setFillColor(sf::Color(20, 20, 20, 200));//dark grey
	m_itemStatsPanel.setOrigin(0, 0);
	m_itemStatsPanel.setPosition(HUD_LEFT_MARGIN, (3 * ARMOR_BAR_SIZE_Y) + 40);

	try
	{
		m_font = new sf::Font();
		if (!m_font->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}//TODO: font loader
		
		m_itemStatsText.setFont(*m_font);
		m_itemStatsText.setCharacterSize(18);
		sf::Color _white = sf::Color::Color(255, 255, 255, 200);//semi-transparent white
		m_itemStatsText.setColor(_white);
		m_itemStatsText.setPosition(HUD_LEFT_MARGIN + 5, (3 * ARMOR_BAR_SIZE_Y) + 40 + 5);
		
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
		m_textShopOptions[ShopBuy].setString("\n\nBuy/Sell");
		m_textShopOptions[StellarMap].setString("\n\nStellar map");

		ostringstream ss_helpNavigation;
		ss_helpNavigation << "\n\n\nFire: select\nBrake: down\nHyperspeed: up";
		m_textHelpNavigation.setFont(*m_font);
		m_textHelpNavigation.setCharacterSize(18);
		m_textHelpNavigation.setColor(_yellow);
		m_textHelpNavigation.setPosition(INTERACTION_PANEL_MARGIN_SIDES + INTERACTION_ARROW_WIDTH, INTERACTION_PANEL_MARGIN_TOP + (INTERACTION_INTERLINE * NB_HAZARD_LEVELS));
		m_textHelpNavigation.setString(ss_helpNavigation.str());

		ostringstream ss_helpBuy;
		ss_helpBuy << "\n\nStart: Switch buy/sell.\nFire: buy/sell.\nSlowmotion: exit\n";
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

GameObject* InteractionPanel::UpdateShopInteraction(sf::Time deltaTime)
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

			//stats panel follow cursor
			m_itemStatsPanel.setPosition(sf::Vector2f(m_cursor->getPosition().x - m_itemStatsPanel.getGlobalBounds().width / 2, m_cursor->getPosition().y + ITEM_STATS_SHOP_OFFSET_Y));
			m_itemStatsText.setPosition(sf::Vector2f(5 + m_cursor->getPosition().x - m_itemStatsPanel.getGlobalBounds().width / 2, 5 + m_cursor->getPosition().y + ITEM_STATS_SHOP_OFFSET_Y));

			//cursor hovering grid
			m_fakeShopGrid.CleanFocus();//reset previous highlight
			int hovered_index_ = m_fakeShopGrid.isCursorColliding(*m_cursor);
			if (hovered_index_ > -1)//the grid is hovered
			{
				m_fakeShopGrid.HighlightCell(hovered_index_);
				m_focused_item = m_shopGrid.getCellPointerFromIntIndex(hovered_index_);
				if (m_focused_item != NULL)
				{
					m_cursor->setAnimationLine(Cursor_ActionState);

					//we want to highlight the equivalent item in HUD (item currently possessed by the player)
					return m_focused_item;
				}
				else
				{
					//empty cell
					m_cursor->setAnimationLine(Cursor_HighlightState);
					return NULL; // m_fakeShopGrid.getCellPointerFromIntIndex(hovered_index_);
				}
			}
			else//no focus at all: the cursor is on an empty space
			{
				m_focused_item = NULL;
				m_cursor->setAnimationLine(Cursor_NormalState);
				return NULL;// m_fakeShopGrid.getCellPointerFromIntIndex(0);
			}
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

	return NULL;
}

sf::Vector2f InteractionPanel::GetShopBuyPanelSize()
{
	return sf::Vector2f((2 * INTERACTION_PANEL_MARGIN_SIDES) + (SHOP_GRID_NB_ROWS * SHOP_GRID_SLOT_SIZE), (2 * INTERACTION_PANEL_MARGIN_TOP) + (SHOP_GRID_NB_LINES * SHOP_GRID_SLOT_SIZE) + m_textDestination.getCharacterSize() + m_textHelpNavigation.getGlobalBounds().height);
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

GameObject* InteractionPanel::Update(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime)
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
		return UpdateShopInteraction(deltaTime);
	}

	return NULL;
}

void InteractionPanel::UpdateItemStatsText(sf::Text* text, int focused_item_type, string f_name, int f_level, int f_xp, float f_max_speed, float f_hyperspeed, int f_armor, int f_shield, int f_shield_regen,
	int f_damage, bool f_bot, float f_ammo_speed, PatternType f_pattern,
	int f_multishot, int f_xspread, float f_rate_of_fire, ShotMode f_shot_mode, float f_dispersion, int f_rafale, float f_rafale_cooldown, TargetSeaking f_target_seaking)
{
	//ITEM STATS PANEL DISPLAY
	ostringstream ss_stats;
	//if focused item != NULL
	switch (focused_item_type)
	{
		case Engine:
		{
						ss_stats << "THRUSTER: " << f_name << "\nSpeed: " << f_max_speed << "\nHyperspeed: " << f_hyperspeed << "\nContact damage: " << f_damage;
						break;
		}
		case Armor:
		{
						ss_stats << "HULL: " << f_name << "\nHull pts: " << f_armor;
						break;
		}
		case Shield:
		{
						ss_stats << "SHIELD: " << f_name << "\nMax shield pts: " << f_shield << "\nShield regen/sec: " << f_shield_regen;
						break;
		}
		case Module:
		{
						ss_stats << "MODULE: " << f_name;
						if (f_bot)
						{
							ss_stats << " \nAdding 1 drone. Drone stats:";
							if (f_shot_mode != NoShotMode)
							{
								ss_stats << "\nDPS: " << (floor)(1 / f_rate_of_fire * 100) / 100 * f_damage;
							}
							else
							{
								ss_stats << "\nDPS: " << (floor)(1 / f_rate_of_fire * 100) / 100 * f_multishot * f_damage;
							}

							ss_stats << "\nDamage: " << f_damage;
							ss_stats << "\nAmmo speed: " << f_ammo_speed;
							ss_stats << "\nFire rate: " << (floor)(1 / f_rate_of_fire * 100) / 100 << " shots/sec";

							if (f_multishot > 1)
							{
								ss_stats << "\nMultishot: " << f_multishot << "\nSpread: " << f_xspread << "\nDispersion: " << f_dispersion << "°";
							}
							else
							{
								ss_stats << "\nSingle shot";
							}
							if (f_rafale > 0)
							{
								ss_stats << "\nRafale: " << f_rafale << " (cooldown: " << f_rafale_cooldown << " sec";
							}

							if (f_shot_mode != NoShotMode)
							{
								ss_stats << "\nFiring style: ";
								switch (f_shot_mode)
								{
									case AlternateShotMode:
									{
																ss_stats << "Alternating shots";
																break;
									}
									case AscendingShotMode:
									{
																ss_stats << "Ascending shots";
																break;
									}
									case DescendingShotMode:
									{
																ss_stats << "Descending shots";
																break;
									}
								}
							}

							if (f_target_seaking != NO_SEAKING)
							{
								switch (f_target_seaking)
								{
									case SEAKING:
									case SUPER_SEAKING:
									{
															ss_stats << "\nSeaking target";
															break;
									}
									case SEMI_SEAKING:
									{
														ss_stats << "\nSeaking target once per rafale";
														break;
									}
								}
							}
						}
						else
						{
							ss_stats << "\nNo effect";
						}
						break;
		}
		case NBVAL_Equipment:
		{
								ss_stats << "MAIN WEAPON: " << f_name;
								if (f_shot_mode != NoShotMode)
								{
									ss_stats << "\nDPS: " << (floor)(1 / f_rate_of_fire * 100) / 100 * f_damage;
								}
								else
								{
									ss_stats << "\nDPS: " << (floor)(1 / f_rate_of_fire * 100) / 100 * f_multishot * f_damage;
								}
								ss_stats << "\nDamage: " << f_damage;
								ss_stats << "\nAmmo speed: " << f_ammo_speed;
								ss_stats << "\nFire rate: " << (floor)(1 / f_rate_of_fire * 100) / 100 << " shots/sec";

								if (f_multishot > 1)
								{
									ss_stats << "\nMultishot: " << f_multishot << "\nSpread: " << f_xspread << "\nDispersion: " << f_dispersion << "°";
								}
								else
								{
									ss_stats << "\nSingle shot";
								}
								if (f_rafale > 0)
								{
									ss_stats << "\nRafale: " << f_rafale << " (cooldown: " << f_rafale_cooldown << " sec";
								}
								if (f_shot_mode != NoShotMode)
								{
									ss_stats << "\nFiring style: ";
									switch (f_shot_mode)
									{
										case AlternateShotMode:
										{
																	ss_stats << "Alternating shots";
																	break;
										}
										case AscendingShotMode:
										{
																	ss_stats << "Ascending shots";
																	break;
										}
										case DescendingShotMode:
										{
																	ss_stats << "Descending shots";
																	break;
										}
									}
								}

								if (f_target_seaking != NO_SEAKING)
								{
									switch (f_target_seaking)
									{
										case SEAKING:
										case SUPER_SEAKING:
										{
																ss_stats << "\nSeaking target";
																break;
										}
										case SEMI_SEAKING:
										{
																ss_stats << "\nSeaking target once per rafale";
																break;
										}
									}
								}
								break;
		}
	}
	ss_stats << "\nLevel: " << f_level << " (+" << f_xp << " XP)";
	ss_stats << "\nMoney value: " << f_xp * MONEY_COST_OF_LOOT_CREDITS;
	
	text->setString(ss_stats.str());
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
			if (m_cursor && m_cursor->m_visible)
			{
				screen.draw(*m_cursor);
			}
		
			if (m_focused_item)
			{
				screen.draw(m_itemStatsPanel);
				screen.draw(m_itemStatsText);
			}
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