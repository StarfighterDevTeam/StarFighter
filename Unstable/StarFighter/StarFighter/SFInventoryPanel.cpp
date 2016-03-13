#include "SFInventoryPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size) : SFPanel(size, SFPanel_ItemStats)
{
	if (object)
	{
		setSize(size);
		setOrigin(size.x/2, size.y/2);
		setFillColor(sf::Color(20, 20, 20, 230));//dark grey
		setOutlineThickness(0);
		setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);

		//texts
		m_title_text.setCharacterSize(18);
		m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

		DisplayItemStats(object);

		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	}
}

void SFItemStatsPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);
	}
}

void SFItemStatsPanel::DisplayItemStats(GameObject* object)
{
	if (object)
	{
		int focused_item_type = object->m_weapon_loot ? NBVAL_Equipment : object->m_equipment_loot->m_equipmentType;
		ostringstream ss_stats;
		ostringstream ss_itam_name;
		//if focused item != NULL
		switch (focused_item_type)
		{
			case Engine:
			{
						   Equipment* obj = object->m_equipment_loot;
						   ss_itam_name << "THRUSTER: " << obj->m_display_name;
						   ss_stats << "Speed: " << obj->m_max_speed << "\nHyperspeed: " << obj->m_hyperspeed << "\nContact damage: " << obj->m_damage;
						   ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP)";
						   ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
						   break;
			}
			case Armor:
			{
						  Equipment* obj = object->m_equipment_loot;
						  ss_itam_name << "HULL: " << obj->m_display_name;
						  ss_stats << "Hull pts: " << obj->m_armor;
						  ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP)";
						  ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
						  break;
			}
			case Shield:
			{
						   Equipment* obj = object->m_equipment_loot;
						   ss_itam_name << "SHIELD: " << obj->m_display_name;
						   ss_stats << "Max shield pts: " << obj->m_shield << "\nShield regen/sec: " << obj->m_shield_regen;
						   ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP)";
						   ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
						   break;
			}
			case Module:
			{
						   Equipment* obj = object->m_equipment_loot;
						   ss_itam_name << "MODULE: " << obj->m_display_name;
						   if (obj->m_bot)
						   {
							   ss_stats << "Adding 1 drone. Drone stats:";
							   if (obj->m_bot->m_weapon->m_shot_mode != NoShotMode)
							   {
								   ss_stats << "\nDPS: " << (floor)(1 / obj->m_bot->m_weapon->m_rate_of_fire * 100) / 100 * obj->m_bot->m_weapon->m_ammunition->m_damage;
							   }
							   else
							   {
								   ss_stats << "\nDPS: " << (floor)(1 / obj->m_bot->m_weapon->m_rate_of_fire * 100) / 100 * obj->m_bot->m_weapon->m_multishot * obj->m_bot->m_weapon->m_ammunition->m_damage;
							   }

							   ss_stats << "\nDamage: " << obj->m_bot->m_weapon->m_ammunition->m_damage;
							   ss_stats << "\nAmmo speed: " << obj->m_bot->m_weapon->m_ammunition->m_speed.y;
							   ss_stats << "\nFire rate: " << (floor)(1 / obj->m_bot->m_weapon->m_rate_of_fire * 100) / 100 << " shots/sec";

							   if (obj->m_bot->m_weapon->m_multishot > 1)
							   {
								   ss_stats << "\nMultishot: " << obj->m_bot->m_weapon->m_multishot << "\nSpread: " << obj->m_bot->m_weapon->m_xspread << "\nDispersion: " << obj->m_bot->m_weapon->m_dispersion << "°";
							   }
							   else
							   {
								   ss_stats << "\nSingle shot";
							   }
							   if (obj->m_bot->m_weapon->m_rafale > 0)
							   {
								   ss_stats << "\nRafale: " << obj->m_bot->m_weapon->m_rafale << " (cooldown: " << obj->m_bot->m_weapon->m_rafale_cooldown << " sec";
							   }

							   if (obj->m_bot->m_weapon->m_multishot != NoShotMode)
							   {
								   ss_stats << "\nFiring style: ";
								   switch (obj->m_bot->m_weapon->m_shot_mode)
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

							   if (obj->m_bot->m_weapon->m_target_seaking != NO_SEAKING)
							   {
								   switch (obj->m_bot->m_weapon->m_target_seaking)
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
									Weapon* obj = object->m_weapon_loot;
									ss_itam_name << "MAIN WEAPON: " << obj->m_display_name;
									if (obj->m_shot_mode != NoShotMode)
									{
										ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_ammunition->m_damage;
									}
									else
									{
										ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_multishot * obj->m_ammunition->m_damage;
									}
									ss_stats << "\nDamage: " << obj->m_ammunition->m_damage;
									ss_stats << "\nAmmo speed: " << obj->m_ammunition->m_speed.y;
									ss_stats << "\nFire rate: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 << " shots/sec";

									if (obj->m_multishot > 1)
									{
										ss_stats << "\nMultishot: " << obj->m_multishot << "\nSpread: " << obj->m_xspread << "\nDispersion: " << obj->m_dispersion << "°";
									}
									else
									{
										ss_stats << "\nSingle shot";
									}
									if (obj->m_rafale > 0)
									{
										ss_stats << "\nRafale: " << obj->m_rafale << " (cooldown: " << obj->m_rafale_cooldown << " sec";
									}
									if (obj->m_shot_mode != NoShotMode)
									{
										ss_stats << "\nFiring style: ";
										switch (obj->m_shot_mode)
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

									if (obj->m_target_seaking != NO_SEAKING)
									{
										switch (obj->m_target_seaking)
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
									ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP)";
									ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
									break;
			}
		}

		m_title_text.setString(ss_itam_name.str());
		m_text.setString(ss_stats.str());
	}
}

//INVENTORY PANEL
SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Inventory)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_use_two_grids = false;
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(HUD_LEFT_MARGIN + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_fake_grid = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), true);
	m_grid = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false);
	
	if (m_use_two_grids)
	{
		m_fake_grid2 = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), true);
		m_grid2 = ObjectGrid(sf::Vector2f(SHOP_GRID_OFFSET_POS_X, SHOP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false);
	}

	//texts
	if (m_playerShip && m_playerShip->m_targetShop)
	{
		//texts strings
		m_title_text.setString(m_playerShip->m_targetShop->m_display_name);

		//insert shop content in grid
		size_t vectorItemsInShopSize = m_playerShip->m_targetShop->m_items.size();
		for (size_t i = 0; i < vectorItemsInShopSize; i++)
		{
			m_grid.insertObject(*m_playerShip->m_targetShop->m_items[i], i, true);
		}
	}
	ostringstream ss_helpNavigation;
	ss_helpNavigation << "\n\n\nFire: buy\nStart: switch to equipment panel";
	m_actions_text.setString(ss_helpNavigation.str());

	//size and position
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

	//positioning panel content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	text_height += INTERACTION_INTERBLOCK;
	text_height += INTERACTION_INTERBLOCK;
	m_fake_grid.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grid.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	if (m_use_two_grids)
	{
		text_height += INTERACTION_INTERBLOCK;
		m_fake_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		m_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}
	
	text_height += m_fake_grid.squares.y * GRID_SLOT_SIZE + ((int)m_use_two_grids * m_fake_grid2.squares.y * GRID_SLOT_SIZE) + INTERACTION_INTERBLOCK - m_actions_text.getGlobalBounds().height / 2;
	text_height += INTERACTION_INTERBLOCK + INTERACTION_INTERBLOCK;//don't know why but it's currently required to get to the correct position
	m_actions_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	text_height += m_actions_text.getGlobalBounds().height;

	//init cursor position on first cell
	if (m_fake_grid.grid[0][0])
	{
		m_cursor.setPosition(m_fake_grid.grid[0][0]->getPosition().x, m_fake_grid.grid[0][0]->getPosition().y);
	}
}

SFInventoryPanel::~SFInventoryPanel()
{
	if (m_item_stats_panel)
	{
		delete m_item_stats_panel;
		m_item_stats_panel = NULL;
	}
}
void SFInventoryPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	if (m_cursor.m_visible)
	{
		////move cursor
		//if (!m_disable_cursor_move)
		//{
		//	m_cursor.m_speed.x = inputs_directions.x * HUD_CURSOR_SPEED;
		//	m_cursor.m_speed.y = inputs_directions.y * HUD_CURSOR_SPEED;
		//}
		//
		//m_cursor.update(deltaTime);
		//
		////panel constraints
		//sf::Vector2f panel_size = getSize();
		//sf::Vector2f panel_pos = getPosition();
		//sf::Vector2f cursor_pos = m_cursor.getPosition();
		//if (cursor_pos.x < panel_pos.x - (panel_size.x / 2))
		//{
		//	m_cursor.setPosition(panel_pos.x - (panel_size.x / 2), cursor_pos.y);
		//	m_cursor.m_speed.x = 0;
		//}
		//if (cursor_pos.x > panel_pos.x + (panel_size.x / 2))
		//{
		//	m_cursor.setPosition(panel_pos.x + (panel_size.x / 2), cursor_pos.y);
		//	m_cursor.m_speed.x = 0;
		//}
		//if (cursor_pos.y < panel_pos.y - (panel_size.y / 2))
		//{
		//	m_cursor.setPosition(cursor_pos.x, panel_pos.y - (panel_size.y / 2));
		//	m_cursor.m_speed.y = 0;
		//}
		//if (cursor_pos.y > panel_pos.y + (panel_size.y / 2))
		//{
		//	m_cursor.setPosition(cursor_pos.x, panel_pos.y + (panel_size.y / 2));
		//	m_cursor.m_speed.y = 0;
		//}

		//update hovered item and highlight feedbacks
		GetHoveredObjectInGrid(m_grid, m_fake_grid);
		if (m_use_two_grids)
		{
			GetHoveredObjectInGrid(m_grid2, m_fake_grid2);
		}
	}
}

void SFInventoryPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		m_fake_grid.Draw(screen);
		m_grid.Draw(screen);
		if (m_use_two_grids)
		{
			m_fake_grid2.Draw(screen);
			m_grid2.Draw(screen);
		}
		screen.draw(m_title_text);
		screen.draw(m_actions_text);
		if (m_cursor.m_visible)
		{
			screen.draw(m_cursor);
		}
		if (m_item_stats_panel)
		{
			m_item_stats_panel->Draw(screen);
		}
	}
}

GameObject* SFInventoryPanel::GetHoveredObjectInGrid(ObjectGrid grid, ObjectGrid fake_grid)
{
	//reset previous highlight
	for (int i = 0; i < fake_grid.squares.x; i++)
	{
		for (int j = 0; j < fake_grid.squares.y; j++)
		{
			fake_grid.grid[i][j]->setAnimationLine(Slot_NormalState);
		}
	}

	//getting hovered cell's index
	int hovered_index = fake_grid.isCursorColliding(m_cursor);

	if (hovered_index < 0)//case: no cell is hovered
	{
		m_focused_cell_index.x = -1;
		m_focused_cell_index.y = -1;
		m_focused_item = NULL;

		//cursor feedback
		m_cursor.setAnimationLine(Cursor_NormalState);

		//destroy stats panel
		if (m_item_stats_panel)
		{
			delete m_item_stats_panel;
			m_item_stats_panel = NULL;
		}

		return m_focused_item;
	}
	else//case: a cell is hovered
	{
		//update knowledge of focused index
		m_focused_cell_index.x = hovered_index / fake_grid.squares.y;
		m_focused_cell_index.y = hovered_index % fake_grid.squares.y;

		//highlight new focused cell
		fake_grid.SetCellHighlightState(hovered_index, Slot_HighlightState);

		//update focused item
		GameObject* previous_focused_item = m_focused_item;
		m_focused_item = grid.grid[m_focused_cell_index.x][m_focused_cell_index.y];

		//item hovered
		if (m_focused_item)
		{
			m_cursor.setAnimationLine(Cursor_ActionState);

			//update item stats panel
			if (previous_focused_item != m_focused_item)
			{
				delete m_item_stats_panel;
			}
			if (!previous_focused_item || previous_focused_item != m_focused_item)
			{
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y));
			}
		}
		//empty cell
		else
		{
			m_cursor.setAnimationLine(Cursor_HighlightState);
			//destroy stats panels
			if (m_item_stats_panel)
			{
				delete m_item_stats_panel;
				m_item_stats_panel = NULL;
			}
		}

		previous_focused_item = NULL;
		return m_focused_item;
	}
}

GameObject* SFInventoryPanel::GetCursor()
{
	return &m_cursor;
}

GameObject* SFInventoryPanel::GetFocusedItem()
{
	return m_focused_item;
}

ObjectGrid* SFInventoryPanel::GetGrid()
{
	return &m_grid;
}

sf::Vector2i SFInventoryPanel::GetFocusedIndex()
{
	return m_focused_cell_index;
}