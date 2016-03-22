#include "SFInventoryPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, bool item_in_shop) : SFPanel(size, SFPanel_ItemStats)
{
	if (object)
	{
		setSize(size);
		setOrigin(size.x/2, size.y/2);
		setFillColor(sf::Color(20, 20, 20, 230));//dark grey
		setOutlineThickness(0);
		setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);

		m_arrow = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
			sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));
		m_arrow.m_visible = false;

		//texts
		m_title_text.setCharacterSize(18);
		m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

		m_options_text = new sf::Text[3];
		for (size_t i = 0; i < 3; i++)
		{
			m_options_text[i].setFont(*(*CurrentGame).m_font[Font_Arial]);
			m_options_text[i].setCharacterSize(18);
		}

		DisplayItemStats(object);

		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		for (size_t i = 0; i < 3; i++)
		{
			if (i == 0)
			{
				text_height += INTERACTION_INTERBLOCK + m_text.getGlobalBounds().height;
			}
			else
			{
				text_height += INTERACTION_INTERLINE;
			}
			text_height += m_options_text[i].getGlobalBounds().height;
			m_options_text[i].setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);
		}

		if (item_in_shop)
		{
			int price = object->m_equipment_loot ? object->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS : object->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
			ostringstream ss_buy;
			ss_buy << "Buy: $" << price;
			if (playerShip->m_money < price)
			{
				m_options_text[0].setColor(sf::Color(255, 50, 50, 255));//red
				ss_buy << " (insufficient credits)";
			}
			m_options_text[0].setString(ss_buy.str());

			m_arrow.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
			m_arrow.m_visible = true;
		}
		

		
	}
}

SFItemStatsPanel::~SFItemStatsPanel()
{
	delete[] m_options_text;
}

void SFItemStatsPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);
		for (size_t i = 0; i < 3; i++)
		{
			screen.draw(m_options_text[i]);
		}
		screen.draw(m_actions_text);
		if (m_arrow.m_visible)
		{
			screen.draw(m_arrow);
		}
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
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "THRUSTER: " << standard_name;
				ss_stats << "Hyperspeed: " << obj->m_hyperspeed << "\nHyperspeed fuel: " << obj->m_hyperspeed_fuel << "\nContact damage: " << obj->m_damage;
				ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				break;
			}
			case Armor:
			{
				Equipment* obj = object->m_equipment_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "HULL: " << standard_name;
				ss_stats << "Hull pts: " << obj->m_armor;
				ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				break;
			}
			case Shield:
			{
				Equipment* obj = object->m_equipment_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "SHIELD: " << standard_name;
				ss_stats << "Max shield pts: " << obj->m_shield << "\nShield regen/sec: " << obj->m_shield_regen << "\nShield recovery: " << obj->m_shield_recovery_time << "sec";
				ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				break;
			}
			case Module:
			{
				Equipment* obj = object->m_equipment_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "MODULE: " << standard_name;
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

					if (obj->m_bot->m_weapon->m_shot_mode != NoShotMode)
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
				ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				break;
			}
			case NBVAL_Equipment:
			{
				Weapon* obj = object->m_weapon_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "MAIN WEAPON: " << standard_name;
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
				ss_stats << "\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: " << obj->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				break;
			}
		}

		m_title_text.setString(ss_itam_name.str());
		m_text.setString(ss_stats.str());
	}
}

//INVENTORY PANEL
SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip, size_t nb_lines, size_t nb_rows, bool use_two_grids, bool use_grey_if_no_money, size_t nb_lines2, size_t nb_rows2) : SFPanel(size, SFPanel_Inventory)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_use_two_grids = use_two_grids;
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_focused_grid = 0;
	m_has_prioritary_feedback = false;
	m_use_grey_if_no_money = use_grey_if_no_money;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_fake_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), true);
	m_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), false);
	
	if (m_use_two_grids)
	{
		m_fake_grid2 = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines2, nb_rows2), true);
		m_grid2 = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines2, nb_rows2), false);
	}

	if (use_grey_if_no_money)
	{
		m_grey_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), false, true);
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
		text_height += INTERACTION_INTERBLOCK + m_fake_grid.squares.y*GRID_SLOT_SIZE;
		m_fake_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		m_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}

	if (m_use_grey_if_no_money)
	{
		m_grey_grid.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
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
	//update hovered item and highlight feedbacks
	if (!m_use_two_grids)
	{
		GetHoveredObjectInGrid(m_grid, m_fake_grid);
	}
	else
	{
		GetHoveredObjectInTwoGrids(m_grid, m_fake_grid, m_grid2, m_fake_grid2);
	}

	//cover cells with a semi-transparent black if player doesn't have the money required
	//if (m_use_grey_if_no_money)
	{
		for (int i = 0; i < m_grey_grid.squares.x; i++)
		{
			for (int j = 0; j < m_grey_grid.squares.y; j++)
			{
				if (m_grid.grid[i][j])
				{
					if (m_grid.grid[i][j]->m_equipment_loot)
					{
						m_grey_grid.grid[i][j]->m_visible = m_playerShip->m_money < m_grid.grid[i][j]->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					}
					else if (m_grid.grid[i][j]->m_weapon_loot)
					{
						m_grey_grid.grid[i][j]->m_visible = m_playerShip->m_money < m_grid.grid[i][j]->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					}
				}
			}
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
		m_grey_grid.Draw(screen);

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
	fake_grid.ClearHighlight();

	//getting hovered cell's index
	int hovered_index = fake_grid.isCursorColliding(m_cursor);

	if (hovered_index < 0)//case: no cell is hovered
	{
		m_focused_cell_index.x = -1;
		m_focused_cell_index.y = -1;
		m_focused_item = NULL;

		//cursor feedback
		if (!m_has_prioritary_feedback)
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
			if (!m_has_prioritary_feedback)
				m_cursor.setAnimationLine(Cursor_ActionState);

			//update item stats panel
			if (previous_focused_item != m_focused_item)
			{
				delete m_item_stats_panel;
			}
			if (!previous_focused_item || previous_focused_item != m_focused_item)
			{
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, this == m_playerShip->m_SFPanel);
			}
		}
		//empty cell
		else
		{
			if (!m_has_prioritary_feedback)
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

GameObject* SFInventoryPanel::GetHoveredObjectInTwoGrids(ObjectGrid grid, ObjectGrid fake_grid, ObjectGrid grid2, ObjectGrid fake_grid2)
{
	//reset previous highlight
	fake_grid.ClearHighlight();
	fake_grid2.ClearHighlight();

	//getting hovered cell's index
	int hovered_index = fake_grid.isCursorColliding(m_cursor);

	if (hovered_index < 0)//check grid 2
	{
		hovered_index = fake_grid2.isCursorColliding(m_cursor);
		if (hovered_index >= 0)
		{
			m_focused_grid = 2;//identify which grid is focused
		}
	}
	else
	{
		m_focused_grid = 1;//identify which grid is focused
	}


	if (hovered_index < 0)//case: no cell is hovered
	{
		m_focused_grid = 0;//identify which grid is focused
		m_focused_cell_index.x = -1;
		m_focused_cell_index.y = -1;
		m_focused_item = NULL;

		//cursor feedback
		if (!m_has_prioritary_feedback)
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
		ObjectGrid* focused_fake_grid = m_focused_grid == 1 ? &fake_grid : &fake_grid2;
		ObjectGrid* focused_grid = m_focused_grid == 1 ? &grid : &grid2;

		m_focused_cell_index.x = hovered_index / focused_fake_grid->squares.y;
		m_focused_cell_index.y = hovered_index % focused_fake_grid->squares.y;

		//highlight new focused cell
		focused_fake_grid->SetCellHighlightState(hovered_index, Slot_HighlightState);

		//update focused item
		GameObject* previous_focused_item = m_focused_item;
		m_focused_item = focused_grid->grid[m_focused_cell_index.x][m_focused_cell_index.y];

		//item hovered
		if (m_focused_item)
		{
			if (!m_has_prioritary_feedback)
				m_cursor.setAnimationLine(Cursor_ActionState);

			//update item stats panel
			if (previous_focused_item != m_focused_item)
			{
				delete m_item_stats_panel;
			}
			if (!previous_focused_item || previous_focused_item != m_focused_item)
			{
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, this == m_playerShip->m_SFPanel);
			}
		}
		//empty cell
		else
		{
			if (!m_has_prioritary_feedback)
				m_cursor.setAnimationLine(Cursor_HighlightState);
			//destroy stats panels
			if (m_item_stats_panel)
			{
				delete m_item_stats_panel;
				m_item_stats_panel = NULL;
			}
		}

		previous_focused_item = NULL;
		focused_grid = NULL;
		return m_focused_item;
	}
}

void SFInventoryPanel::ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid)
{
	if (focused_object && fake_grid)
	{
		int equipment_type = focused_object->m_weapon_loot ? NBVAL_Equipment : focused_object->m_equipment_loot->m_equipmentType;
		if (fake_grid->grid[0][equipment_type])
		{
			m_cursor.setPosition(fake_grid->grid[0][equipment_type]->getPosition());
		}
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

void SFInventoryPanel::SetFocusedItem(GameObject* item)
{
	m_focused_item = item;
}

bool SFInventoryPanel::GetPrioritaryFeedback()
{
	return m_has_prioritary_feedback;
}

void SFInventoryPanel::SetPrioritaryFeedback(bool has_priotiary_feedback)
{
	m_has_prioritary_feedback = has_priotiary_feedback;
}

ObjectGrid* SFInventoryPanel::GetGrid(bool fake_grid, size_t grid)
{
	if (grid == 2 && m_use_two_grids)
	{
		if (fake_grid)
			return &m_fake_grid2;
		else
			return &m_grid2;
	}
	else
	{
		if (fake_grid)
			return &m_fake_grid;
		else
			return &m_grid;
	}
}

void SFInventoryPanel::ClearHighlight()
{
	m_fake_grid.ClearHighlight();
	if (m_use_two_grids)
	{
		m_fake_grid2.ClearHighlight();
	}
}

sf::Vector2i SFInventoryPanel::GetFocusedIndex()
{
	return m_focused_cell_index;
}

int SFInventoryPanel::GetFocusedGrid()
{
	return m_focused_grid;
}

//HUD PANEL
SFHUDPanel::SFHUDPanel(sf::Vector2f size, Ship* playerShip) : SFInventoryPanel(size, playerShip, SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS, true, false, EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS)
{
	setOrigin(0, 0);
	setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	setOutlineThickness(0);
	setPosition(SCENE_SIZE_X, 0);
	m_cursor.m_visible = false;

	if (playerShip)
	{
		//int gauges and texts
		//m_armorBar.setSize(sf::Vector2f(1 + playerShip->m_armor_max, ARMOR_BAR_SIZE_Y));
		m_armorBar.setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
		m_armorBar.setOrigin(0, 0);

		//m_shieldBar.setSize(sf::Vector2f(1 + playerShip->m_shield_max, SHIELD_BAR_SIZE_Y));
		m_shieldBar.setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
		m_shieldBar.setOrigin(0, 0);

		m_fuelBar.setFillColor(sf::Color(COLOR_YELLOW_R_VALUE, COLOR_YELLOW_G_VALUE, COLOR_YELLOW_B_VALUE, COLOR_YELLOW_A_VALUE));//yellow
		m_fuelBar.setOrigin(0, 0);

		//m_armorBarContainer.setSize(sf::Vector2f(1 + playerShip->m_armor_max, ARMOR_BAR_SIZE_Y));
		m_armorBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
		m_armorBarContainer.setOutlineThickness(1);
		m_armorBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_armorBarContainer.setOrigin(0, 0);

		//m_shieldBarContainer.setSize(sf::Vector2f(1 + playerShip->m_shield_max, SHIELD_BAR_SIZE_Y));
		m_shieldBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
		m_shieldBarContainer.setOutlineThickness(1);
		m_shieldBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_shieldBarContainer.setOrigin(0, 0);

		m_fuelBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
		m_fuelBarContainer.setOutlineThickness(1);
		m_fuelBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_fuelBarContainer.setOrigin(0, 0);

		m_xpBar.setSize(sf::Vector2f((1.0f * playerShip->m_xp / playerShip->m_xp_max) * XP_BAR_SIZE_X, XP_BAR_SIZE_Y));
		m_xpBar.setFillColor(sf::Color(COLOR_LIGHT_BLUE_R_VALUE, COLOR_LIGHT_BLUE_G_VALUE, COLOR_LIGHT_BLUE_B_VALUE, COLOR_LIGHT_BLUE_A_VALUE));//light blue
		m_xpBar.setOrigin(0, 0);

		sf::Color _white = sf::Color::Color(255, 255, 255, 200);//semi-transparent white
		m_life_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_life_text.setCharacterSize(10);
		m_life_text.setColor(_white);

		m_shield_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_shield_text.setCharacterSize(10);
		m_shield_text.setColor(_white);

		m_fuel_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_fuel_text.setCharacterSize(10);
		m_fuel_text.setColor(_white);

		m_money_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_money_text.setCharacterSize(20);
		m_money_text.setColor(_white);

		m_graze_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_graze_text.setCharacterSize(14);
		m_graze_text.setColor(_white);

		m_level_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_level_text.setCharacterSize(14);
		m_level_text.setColor(_white);

		m_scene_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_scene_text.setCharacterSize(14);
		m_scene_text.setColor(_white);

		m_framerate_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_framerate_text.setCharacterSize(15);
		m_framerate_text.setColor(sf::Color::Yellow);

		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

		//positioning panel content
		float text_height = 0;
		text_height += 10;
		m_armorBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		m_armorBarContainer.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, 10);
		m_life_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + ARMOR_BAR_SIZE_X / 2, 10);

		text_height += 10 + ARMOR_BAR_SIZE_Y;
		m_shield_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + ARMOR_BAR_SIZE_X / 2, text_height);
		m_shieldBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		m_shieldBarContainer.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += 10 + ARMOR_BAR_SIZE_Y;
		m_fuel_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + ARMOR_BAR_SIZE_X / 2, text_height);
		m_fuelBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		m_fuelBarContainer.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		
		text_height += 10 + ARMOR_BAR_SIZE_Y;//+20?
		m_xpBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_INTERBLOCK + ARMOR_BAR_SIZE_Y;
		m_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += ITEM_STATS_PANEL_SIZE_Y;
		
		m_fake_grid.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_grid.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));

		text_height += m_fake_grid.squares.x * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
		m_fake_grid2.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_grid2.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));

		text_height += m_fake_grid2.squares.x * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
		m_money_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_money_text.getCharacterSize();
		m_level_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_level_text.getCharacterSize();
		m_scene_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_scene_text.getCharacterSize();
		m_graze_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_graze_text.getCharacterSize();
		m_framerate_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
	}
	
	//init cursor position on first cell
	if (m_fake_grid.grid[0][0])
	{
		m_cursor.setPosition(m_fake_grid.grid[0][0]->getPosition().x, m_fake_grid.grid[0][0]->getPosition().y);
	}
}

void SFHUDPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	SFInventoryPanel::Update(deltaTime, inputs_directions);

	
	if (!m_playerShip)
	{
		return;
	}

	//armor
	if (m_playerShip->m_armor_max <= 0)
	{
		m_armorBar.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
		m_armorBarContainer.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
	}
	else
	{
		if (m_playerShip->m_armor_max < m_playerShip->m_shield_max)
		{
			m_armorBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_armor / m_playerShip->m_armor_max * ARMOR_BAR_SIZE_X * m_playerShip->m_armor_max / m_playerShip->m_shield_max), ARMOR_BAR_SIZE_Y));
			m_armorBarContainer.setSize(sf::Vector2f(1 + 1.0f * ARMOR_BAR_SIZE_X * m_playerShip->m_armor_max / m_playerShip->m_shield_max, ARMOR_BAR_SIZE_Y));
		}
		else
		{
			m_armorBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_armor / m_playerShip->m_armor_max * ARMOR_BAR_SIZE_X), ARMOR_BAR_SIZE_Y));
			m_armorBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, ARMOR_BAR_SIZE_Y));
		}
	}

	//shield
	if (m_playerShip->m_shield_max <= 0)
	{
		m_shieldBar.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
		m_shieldBarContainer.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
	}
	else
	{
		if (m_playerShip->m_shield_max < m_playerShip->m_armor_max)
		{
			m_shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_shield / m_playerShip->m_shield_max * ARMOR_BAR_SIZE_X * m_playerShip->m_shield_max / m_playerShip->m_armor_max), SHIELD_BAR_SIZE_Y));
			m_shieldBarContainer.setSize(sf::Vector2f(1 + 1.0f * ARMOR_BAR_SIZE_X * m_playerShip->m_shield_max / m_playerShip->m_armor_max, SHIELD_BAR_SIZE_Y));
		}
		else
		{
			m_shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_shield / m_playerShip->m_shield_max * ARMOR_BAR_SIZE_X), SHIELD_BAR_SIZE_Y));
			m_shieldBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, SHIELD_BAR_SIZE_Y));
		}
	}

	//fuel
	if (m_playerShip->m_hyperspeed_fuel_max <= 0)
	{
		m_fuelBar.setSize(sf::Vector2f(1, FUEL_BAR_SIZE_Y));
		m_fuelBarContainer.setSize(sf::Vector2f(1, FUEL_BAR_SIZE_Y));
	}
	else
	{
		m_fuelBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_hyperspeed_fuel / m_playerShip->m_hyperspeed_fuel_max * ARMOR_BAR_SIZE_X), FUEL_BAR_SIZE_Y));
		m_fuelBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, FUEL_BAR_SIZE_Y));
	}

	ostringstream ss_life;
	ss_life << m_playerShip->m_armor << "/" << m_playerShip->m_armor_max;
	m_life_text.setString(ss_life.str());
	m_life_text.setPosition(m_armorBar.getPosition().x + m_armorBar.getSize().x / 2 - m_life_text.getGlobalBounds().width/2, m_armorBar.getPosition().y + m_life_text.getGlobalBounds().height / 2);

	ostringstream ss_shield;
	ss_shield << m_playerShip->m_shield << "/" << m_playerShip->m_shield_max;
	m_shield_text.setString(ss_shield.str());
	m_shield_text.setPosition(m_shieldBar.getPosition().x + m_shieldBar.getSize().x / 2 - m_shield_text.getGlobalBounds().width / 2, m_shieldBar.getPosition().y + m_shield_text.getGlobalBounds().height / 2);

	ostringstream ss_fuel;
	ss_fuel << (int)m_playerShip->m_hyperspeed_fuel << "/" << m_playerShip->m_hyperspeed_fuel_max;
	m_fuel_text.setString(ss_fuel.str());
	m_fuel_text.setPosition(m_fuelBar.getPosition().x + m_fuelBar.getSize().x / 2 - m_fuel_text.getGlobalBounds().width / 2, m_fuelBar.getPosition().y + 1);//because it works :/

	//level
	ostringstream ss_slash;
	ss_slash << "Level " << m_playerShip->m_level;
	if (m_playerShip->m_level_max > -1)
	{
		ss_slash << " / " << m_playerShip->m_level_max;
	}
	ss_slash << " (XP: " << m_playerShip->m_xp << " / " << m_playerShip->m_xp_max << ")";
	m_level_text.setString(ss_slash.str());

	m_xpBar.setSize(sf::Vector2f((1.0f * m_playerShip->m_xp / m_playerShip->m_xp_max) * XP_BAR_SIZE_X, SHIELD_BAR_SIZE_Y));
	m_level_text.setPosition(m_level_text.getPosition().x, m_xpBar.getPosition().y);

	//money
	ostringstream ss_m;
	ss_m << m_playerShip->m_money;
	m_money_text.setString(ss_m.str() + "$");

	//graze
	ostringstream ss_g;
	ss_g << m_playerShip->m_graze_count;
	m_graze_text.setString("Graze: " + ss_g.str());

	//scene name
	if (!m_playerShip->m_currentScene_name.empty())
	{
		ostringstream ss_bg;
		ss_bg << m_playerShip->m_currentScene_name;
		if ((*CurrentGame).m_direction != NO_DIRECTION)
		{
			ss_bg << " (" << m_playerShip->m_currentScene_hazard + 1 << ")";
		}
		m_scene_text.setString(ss_bg.str());
	}

	//framerate
	ostringstream ss_frame;
	ss_frame << "fps= " << (int)(1 / (deltaTime.asMilliseconds() * 0.001));
	m_framerate_text.setString(ss_frame.str());

	//ship global stats
	ostringstream ss_ship_stats;
	float DPS = 0;
	if (m_playerShip->m_weapon)
	{
		if (m_playerShip->m_weapon->m_shot_mode != NoShotMode)
		{
			DPS += (floor)(1 / m_playerShip->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_weapon->m_ammunition->m_damage;
		}
		else
		{
			DPS += (floor)(1 / m_playerShip->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_weapon->m_ammunition->m_damage * m_playerShip->m_weapon->m_multishot;
		}
		
	}
	if (!m_playerShip->m_bot_list.empty())
	{
		size_t botsVectorSize = m_playerShip->m_bot_list.size();
		for (size_t i = 0; i < botsVectorSize; i++)
		{
			if (m_playerShip->m_bot_list[i]->m_weapon)
			{
				if (m_playerShip->m_bot_list[i]->m_weapon->m_shot_mode != NoShotMode)
				{
					DPS += (floor)(1 / m_playerShip->m_bot_list[i]->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_bot_list[i]->m_weapon->m_ammunition->m_damage;
				}
				else
				{
					DPS += (floor)(1 / m_playerShip->m_bot_list[i]->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_bot_list[i]->m_weapon->m_ammunition->m_damage *m_playerShip->m_bot_list[i]->m_weapon->m_multishot;
				} 
			}
		}
	}

	ss_ship_stats << "DPS: " << DPS;
	ss_ship_stats << "\nContact damage: " << m_playerShip->m_damage << "\nHyperspeed: " << m_playerShip->m_hyperspeed << "\nFuel: " << m_playerShip->m_hyperspeed_fuel_max
		<< "\nShield regen: " << m_playerShip->m_shield_regen << "/sec" << "\nShield recovery: " << m_playerShip->m_shield_recovery_time << "sec";
	m_text.setString(ss_ship_stats.str());
}

void SFHUDPanel::Draw(sf::RenderTexture& screen)
{
	sf::RectangleShape black_background;
	black_background.setSize(this->getSize());
	black_background.setOrigin(this->getOrigin());
	black_background.setPosition(this->getPosition());
	black_background.setFillColor(sf::Color(0, 0, 0, 255));
	screen.draw(black_background);

	SFInventoryPanel::Draw(screen);

	screen.draw(m_armorBarContainer);
	screen.draw(m_armorBar);
	screen.draw(m_life_text);
	if (m_playerShip && m_playerShip->m_shield_max > 0)
	{
		screen.draw(m_shieldBarContainer);
		screen.draw(m_shieldBar);
		screen.draw(m_shield_text);
	}
	if (m_playerShip && m_playerShip->m_hyperspeed_fuel_max > 0)
	{
		screen.draw(m_fuelBarContainer);
		screen.draw(m_fuelBar);
		screen.draw(m_fuel_text);
	}

	screen.draw(m_money_text);
	screen.draw(m_graze_text);
	screen.draw(m_xpBar);
	screen.draw(m_level_text);
	screen.draw(m_scene_text);
	screen.draw(m_framerate_text);
	screen.draw(m_text);
}