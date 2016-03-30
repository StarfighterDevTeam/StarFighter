#include "SFInventoryPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, FocusedItemStates item_state) : SFPanel(size, SFPanel_ItemStats)
{
	if (object)
	{
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		setFillColor(sf::Color(20, 20, 20, 230));//dark grey
		setOutlineThickness(0);
		setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);
		m_selected_option_index = 0;

		m_arrow = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_ARROW_FILENAME, sf::Vector2f(INTERACTION_ARROW_WIDTH, INTERACTION_ARROW_HEIGHT),
			sf::Vector2f(INTERACTION_ARROW_WIDTH / 2, INTERACTION_ARROW_HEIGHT / 2));
		m_arrow.m_visible = true;

		//texts
		m_title_text.setCharacterSize(18);
		m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_options_text[0].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_options_text[0].setCharacterSize(18);
		m_options_text[1].setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_options_text[1].setCharacterSize(18);

		DisplayItemStats(object);

		float text_height = 0;
		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_text.getGlobalBounds().height;

		//Action texts
		//BUY
		if (item_state == FocusedItem_Buy)
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
			m_number_of_options = 1;
		}
		//SELL
		else if (item_state == FocusedItem_SellOrEquip || item_state == FocusedItem_SellOrDesequip)
		{
			int price = object->m_equipment_loot ? object->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS : object->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
			ostringstream ss_sell;
			ss_sell << "Sell: $" << price;
			m_options_text[0].setString(ss_sell.str());
			m_number_of_options = 2;

			//SELL / EQUIP
			if (item_state == FocusedItem_SellOrEquip)
			{
				m_options_text[1].setString("Equip");
			}
			//SELL / DESEQUIP
			else if (item_state == FocusedItem_SellOrDesequip)
			{
				m_options_text[1].setString("Desequip");
			}
		}
		//EQUIP
		else if (item_state == FocusedItem_Equip)
		{
			m_options_text[0].setString("Equip");
			m_number_of_options = 1;
		}
		//DESEQUIP
		else if (item_state == FocusedItem_Desequip)
		{
			m_options_text[0].setString("Desequip");
			m_number_of_options = 1;
		}

		//options texts
		for (size_t i = 0; i < 2; i++)
		{
			text_height += INTERACTION_INTERLINE;
			text_height += m_options_text[i].getGlobalBounds().height;
			m_options_text[i].setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + m_arrow.m_size.x - (getSize().x / 2), getPosition().y - getSize().y / 2 + text_height);
			if (i == 0)
			{
				m_arrow.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height + m_options_text[0].getCharacterSize() - 6);//because fuck this
			}
		}
	}
}

SFItemStatsPanel::~SFItemStatsPanel()
{

}

void SFItemStatsPanel::Update()
{
	m_arrow.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES - (getSize().x / 2), m_options_text[m_selected_option_index].getPosition().y + m_options_text[m_selected_option_index].getCharacterSize() - 6);//because fuck this
}

void SFItemStatsPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		screen.draw(m_text);
		for (size_t i = 0; i < 2; i++)
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
		ss_stats << fixed;
		ss_stats.precision(0);
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
				ss_stats << "Max shield pts: " << obj->m_shield << "\nShield regen/sec: ";
				ss_stats.precision(1);
				ss_stats << obj->m_shield_regen << "\nShield recovery: ";
				ss_stats.precision(0);
				ss_stats << obj->m_shield_recovery_time << "sec";
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
					ss_stats.precision(1);
					ss_stats << "\nFire rate: " << (floor)(1 / obj->m_bot->m_weapon->m_rate_of_fire * 100) / 100 << " shots/sec";
					ss_stats.precision(0);

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
					ss_stats.precision(0);
					ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_ammunition->m_damage;
				}
				else
				{
					ss_stats.precision(0);
					ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_multishot * obj->m_ammunition->m_damage;
				}
				ss_stats << "\nDamage: " << obj->m_ammunition->m_damage;
				ss_stats << "\nAmmo speed: " << obj->m_ammunition->m_speed.y;
				ss_stats.precision(1);
				ss_stats << "\nFire rate: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 << " shots/sec";
				ss_stats.precision(0);

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
SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip, size_t nb_lines, size_t nb_rows, bool use_two_grids, bool is_shop, size_t nb_lines2, size_t nb_rows2) : SFPanel(size, SFPanel_Inventory)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_use_two_grids = use_two_grids;
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_focused_grid = 0;
	m_has_prioritary_feedback = false;
	m_is_shop = is_shop;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));
	m_cursor.m_visible = true;

	m_fake_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), true);
	m_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), false);
	m_quality_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), false, false, true);
	
	if (m_use_two_grids)
	{
		m_fake_grid2 = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines2, nb_rows2), true);
		m_grid2 = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines2, nb_rows2), false);
		m_quality_grid2 = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines2, nb_rows2), false, false, true);
	}

	if (is_shop)
	{
		m_grey_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(nb_lines, nb_rows), false, true);
	}

	//texts
	if (is_shop && m_playerShip && m_playerShip->m_targetShop)
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

	m_quality_grid.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	if (m_use_two_grids)
	{
		text_height += INTERACTION_INTERBLOCK + m_fake_grid.squares.y*GRID_SLOT_SIZE;
		m_fake_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		m_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
		m_quality_grid2.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}

	if (m_is_shop)
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
	//update item stats panel
	if (m_item_stats_panel)
	{
		m_item_stats_panel->Update();
	}

	//update background colors
	UpdateBackgroundColors(m_quality_grid, m_grid);

	//update hovered item and highlight feedbacks
	if (!m_use_two_grids)
	{
		GetHoveredObjectInGrid(m_grid, m_fake_grid);
	}
	else
	{
		GetHoveredObjectInTwoGrids(m_grid, m_fake_grid, m_grid2, m_fake_grid2);
		UpdateBackgroundColors(m_quality_grid2, m_grid2);
	}

	//cover cells with a semi-transparent black if player doesn't have the money required
	//if (m_is_shop)
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

void SFInventoryPanel::UpdateBackgroundColors(ObjectGrid color_grid, ObjectGrid object_grid)
{
	for (int i = 0; i < color_grid.squares.x; i++)
	{
		for (int j = 0; j < color_grid.squares.y; j++)
		{
			if (color_grid.grid[i][j] && i < object_grid.squares.x && j < object_grid.squares.y)
			{
				if (object_grid.grid[i][j])
				{
					float quality = object_grid.grid[i][j]->m_equipment_loot ? object_grid.grid[i][j]->m_equipment_loot->m_quality : (object_grid.grid[i][j]->m_weapon_loot ? object_grid.grid[i][j]->m_weapon_loot->m_quality : -1);
					if (quality < 0)
					{
						color_grid.grid[i][j]->m_visible = false;
						printf("Error in SFInventoryPanel::UpdateBackgroundColors(): trying to get the quality of an item that doesn't have any equipment or weapon.\n");
					}
					else
					{
						color_grid.grid[i][j]->setAnimationLine((int)GetItemQualityClass(quality));
						color_grid.grid[i][j]->m_visible = true;
					}
				}
				else
				{
					color_grid.grid[i][j]->m_visible = false;
				}
			}
			else if (color_grid.grid[i][j])
			{
				color_grid.grid[i][j]->m_visible = false;
			}
		}
	}
}

EquipmentQuality SFInventoryPanel::GetItemQualityClass(float quality)
{
	if (quality < 25)
	{
		return ItemQuality_Poor;
	}
	else if (quality < 50)
	{
		return ItemQuality_Medium;
	}
	else if (quality < 75)
	{
		return ItemQuality_Good;
	}
	else 
	{
		return ItemQuality_Epic;
	}
}

void SFInventoryPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		m_fake_grid.Draw(screen);
		m_quality_grid.Draw(screen);
		m_grid.Draw(screen);
		
		if (m_use_two_grids)
		{
			m_fake_grid2.Draw(screen);
			m_quality_grid2.Draw(screen);
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
		m_focused_grid = 0;
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
		m_focused_grid = 1;
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
				FocusedItemStates item_state = this == m_playerShip->m_SFHudPanel ? (m_focused_grid == 1 ? FocusedItem_Desequip : FocusedItem_Equip) : (m_focused_grid == Trade_ShopGrid ? FocusedItem_Buy : (m_focused_grid == Trade_EquippedGrid ? FocusedItem_SellOrDesequip : FocusedItem_SellOrEquip));
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state);
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
				FocusedItemStates item_state = this == m_playerShip->m_SFHudPanel ? (m_focused_grid == 1 ? FocusedItem_Desequip : FocusedItem_Equip) : (m_focused_grid == Trade_ShopGrid ? FocusedItem_Buy : (m_focused_grid == Trade_EquippedGrid ? FocusedItem_SellOrDesequip : FocusedItem_SellOrEquip));
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state);
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

SFItemStatsPanel* SFInventoryPanel::GetItemStatsPanel()
{
	return m_item_stats_panel;
}

void SFInventoryPanel::SetItemStatsPanel(SFItemStatsPanel* panel)
{
	m_item_stats_panel = panel;
}

void SFInventoryPanel::SetPrioritaryFeedback(bool has_priotiary_feedback)
{
	m_has_prioritary_feedback = has_priotiary_feedback;
}

ObjectGrid* SFInventoryPanel::GetGrid(bool fake_grid, int grid)
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

int SFInventoryPanel::GetFocusedIntIndex()
{
	if (m_focused_grid == 1)
	{
		return m_focused_cell_index.y + (m_focused_cell_index.x * m_grid.squares.y);
	}
	else if (m_focused_grid == 2)
	{
		return m_focused_cell_index.y + (m_focused_cell_index.x * m_grid2.squares.y);
	}
	else
	{
		return -1;//error code
	}
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

		m_hazardscore_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hazardscore_text.setCharacterSize(14);
		m_hazardscore_text.setColor(_white);

		m_beastscore_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_beastscore_text.setCharacterSize(14);
		m_beastscore_text.setColor(_white);

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

		text_height += ITEM_STATS_PANEL_SIZE_Y - INTERACTION_INTERBLOCK - INTERACTION_INTERLINE;//because fuck this
		
		m_fake_grid.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_grid.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_quality_grid.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));

		text_height += m_fake_grid.squares.x * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
		m_fake_grid2.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_grid2.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));
		m_quality_grid2.SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y + text_height));

		text_height += m_fake_grid2.squares.x * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
		m_money_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += m_money_text.getCharacterSize();
		m_level_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_level_text.getCharacterSize();
		m_scene_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_scene_text.getCharacterSize();
		m_graze_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_graze_text.getCharacterSize();
		m_hazardscore_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_hazardscore_text.getCharacterSize();
		m_beastscore_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_beastscore_text.getCharacterSize();
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
	m_life_text.setPosition(m_armorBarContainer.getPosition().x + m_armorBarContainer.getSize().x / 2 - m_life_text.getGlobalBounds().width / 2, m_armorBarContainer.getPosition().y + m_life_text.getGlobalBounds().height / 2);

	ostringstream ss_shield;
	ss_shield << m_playerShip->m_shield << "/" << m_playerShip->m_shield_max;
	m_shield_text.setString(ss_shield.str());
	m_shield_text.setPosition(m_shieldBarContainer.getPosition().x + m_shieldBarContainer.getSize().x / 2 - m_shield_text.getGlobalBounds().width / 2, m_shieldBarContainer.getPosition().y + m_shield_text.getGlobalBounds().height / 2);

	ostringstream ss_fuel;
	ss_fuel << (int)m_playerShip->m_hyperspeed_fuel << "/" << m_playerShip->m_hyperspeed_fuel_max;
	m_fuel_text.setString(ss_fuel.str());
	m_fuel_text.setPosition(m_fuelBarContainer.getPosition().x + m_fuelBarContainer.getSize().x / 2 - m_fuel_text.getGlobalBounds().width / 2, m_fuelBarContainer.getPosition().y + 1);//because it works :/

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
	m_money_text.setString("$ " + ss_m.str());

	//graze
	ostringstream ss_g;
	ss_g << m_playerShip->m_graze_count;
	m_graze_text.setString("Graze: " + ss_g.str());

	//score
	ostringstream ss_sc;
	ss_sc << (*CurrentGame).m_hazard << " / " << (*CurrentGame).m_hazardSpawned;
	m_hazardscore_text.setString("Score: " + ss_sc.str());

	//Beast score
	ostringstream ss_beast;
	float quality_graze = m_playerShip->getShipBeastScore() / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES) * 100;
	float quality_hazard = Scene::getSceneBeastScore(m_playerShip->m_currentScene_hazard) / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES) * 100;
	ss_beast.precision(0);
	ss_beast << fixed;
	ss_beast << "Drop quality: +" << quality_graze + quality_hazard << "%";
	ss_beast << ": " << quality_graze << " (graze) +" << quality_hazard << " (hazard)";
	m_beastscore_text.setString(ss_beast.str());

	//scene name
	if (!m_playerShip->m_currentScene_name.empty())
	{
		ostringstream ss_bg;
		ss_bg << m_playerShip->m_currentScene_name;
		if ((*CurrentGame).m_direction != NO_DIRECTION)
		{
			ss_bg << " (" << m_playerShip->m_currentScene_hazard + 1 << ")";
		}
		m_scene_text.setString(ReplaceAll(ss_bg.str(), "_", " "));
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
	screen.draw(m_hazardscore_text);
	screen.draw(m_beastscore_text);
	screen.draw(m_xpBar);
	screen.draw(m_level_text);
	screen.draw(m_scene_text);
	screen.draw(m_framerate_text);
	screen.draw(m_text);
}

//TRADE PANEL
SFTradePanel::SFTradePanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Trade)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_focused_grid = 0;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_title_text2.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_title_text2.setCharacterSize(m_title_text.getCharacterSize());
	m_title_text2.setColor(m_title_text.getColor());
	m_actions_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_fake_grid[Trade_ShopGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), true);
	m_grid[Trade_ShopGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false);
	m_quality_grid[Trade_ShopGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false, false, true);

	m_fake_grid[Trade_StashGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), true);
	m_grid[Trade_StashGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false);
	m_quality_grid[Trade_StashGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false, false, true);

	m_fake_grid[Trade_EquippedGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), true);
	m_grid[Trade_EquippedGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false);
	m_quality_grid[Trade_EquippedGrid] = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false, false, true);

	m_grey_grid = ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false, true);

	//texts
	if (playerShip)
	{
		//player name and content
		m_title_text2.setString(m_playerShip->m_display_name);
		//copy player grids into trade panel
		for (size_t i = 0; i < m_grid[Trade_EquippedGrid].squares.x; i++)
		{
			for (size_t j = 0; j < m_grid[Trade_EquippedGrid].squares.y; j++)
			{
				if (i < playerShip->m_SFHudPanel->GetGrid()->squares.x && j < playerShip->m_SFHudPanel->GetGrid()->squares.y && playerShip->m_SFHudPanel->GetGrid()->grid[i][j])
				{
					m_grid[Trade_EquippedGrid].grid[i][j] = playerShip->m_SFHudPanel->GetGrid()->grid[i][j]->Clone();
				}
			}
		}
		for (size_t i = 0; i < m_grid[Trade_StashGrid].squares.x; i++)
		{
			for (size_t j = 0; j < m_grid[Trade_StashGrid].squares.y; j++)
			{
				if (i < playerShip->m_SFHudPanel->GetGrid(false, 2)->squares.x && j < playerShip->m_SFHudPanel->GetGrid(false, 2)->squares.y && playerShip->m_SFHudPanel->GetGrid(false, 2)->grid[i][j])
				{
					m_grid[Trade_StashGrid].grid[i][j] = playerShip->m_SFHudPanel->GetGrid(false, 2)->grid[i][j]->Clone();
				}
			}
		}

		//shop name and items
		if (m_playerShip->m_targetShop)
		{
			m_title_text.setString(m_playerShip->m_targetShop->m_display_name);

			//insert shop content in grid
			size_t vectorItemsInShopSize = m_playerShip->m_targetShop->m_items.size();
			for (size_t i = 0; i < vectorItemsInShopSize; i++)
			{
				if (m_playerShip->m_targetShop->m_items[i])
				{
					m_grid[Trade_ShopGrid].insertObject(*m_playerShip->m_targetShop->m_items[i], i, true);
				}
			}
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
	m_title_text2.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	text_height += INTERACTION_INTERBLOCK;
	text_height += INTERACTION_INTERBLOCK;

	m_fake_grid[Trade_ShopGrid].SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grid[Trade_ShopGrid].SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_quality_grid[Trade_ShopGrid].SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grey_grid.SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	m_fake_grid[Trade_EquippedGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grid[Trade_EquippedGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_quality_grid[Trade_EquippedGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	float text_height2 = text_height += INTERACTION_INTERBLOCK + m_fake_grid[Trade_EquippedGrid].grid[0][0]->m_size.y;
	m_fake_grid[Trade_StashGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));
	m_grid[Trade_StashGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));
	m_quality_grid[Trade_StashGrid].SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));

	text_height += m_fake_grid[Trade_ShopGrid].squares.y * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK - m_actions_text.getGlobalBounds().height / 2;
	text_height += INTERACTION_INTERBLOCK + INTERACTION_INTERBLOCK;//don't know why but it's currently required to get to the correct position
	m_actions_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);
	text_height += m_actions_text.getGlobalBounds().height;

	//init cursor position on first cell
	m_cursor.setPosition(m_fake_grid[Trade_ShopGrid].grid[0][0]->getPosition().x, m_fake_grid[Trade_ShopGrid].grid[0][0]->getPosition().y);

	//build separator between shop items and player items
	m_separator.setSize(sf::Vector2f(1, size.y));
	m_separator.setOrigin(m_separator.getSize().x / 2, m_separator.getSize().y/2);
	m_separator.setPosition(getPosition());
	m_separator.setFillColor(getOutlineColor());
}

SFTradePanel::~SFTradePanel()
{
	
}

void SFTradePanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//update background colors
	SFInventoryPanel::UpdateBackgroundColors(m_quality_grid[Trade_ShopGrid], m_grid[Trade_ShopGrid]);
	SFInventoryPanel::UpdateBackgroundColors(m_quality_grid[Trade_EquippedGrid], m_grid[Trade_EquippedGrid]);
	SFInventoryPanel::UpdateBackgroundColors(m_quality_grid[Trade_StashGrid], m_grid[Trade_StashGrid]);

	//update hovered item and highlight feedbacks
	GetHoveredObjectInGrid();

	//Update selected index in item stats panel
	if (m_item_stats_panel)
	{
		m_item_stats_panel->Update();
	}

	//cover cells with a semi-transparent black if player doesn't have the money required
	//if (m_is_shop)
	{
		for (int i = 0; i < m_grey_grid.squares.x; i++)
		{
			for (int j = 0; j < m_grey_grid.squares.y; j++)
			{
				if (m_grid[Trade_ShopGrid].grid[i][j])
				{
					if (m_grid[Trade_ShopGrid].grid[i][j]->m_equipment_loot)
					{
						m_grey_grid.grid[i][j]->m_visible = m_playerShip->m_money < m_grid[Trade_ShopGrid].grid[i][j]->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					}
					else if (m_grid[Trade_ShopGrid].grid[i][j]->m_weapon_loot)
					{
						m_grey_grid.grid[i][j]->m_visible = m_playerShip->m_money < m_grid[Trade_ShopGrid].grid[i][j]->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					}
				}
			}
		}
	}
}

void SFTradePanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_separator);
		screen.draw(m_title_text);
		screen.draw(m_title_text2);
		screen.draw(m_actions_text);
		for (size_t i = 0; i < NBVAL_TradeGrids; i++)
		{
			m_fake_grid[i].Draw(screen);
			m_quality_grid[i].Draw(screen);
			m_grid[i].Draw(screen);
		}
		m_grey_grid.Draw(screen);
		if (m_item_stats_panel)
		{
			m_item_stats_panel->Draw(screen);
		}
		if (m_cursor.m_visible)
		{
			screen.draw(m_cursor);
		}
	}
}

GameObject* SFTradePanel::GetHoveredObjectInGrid()
{
	//reset previous highlight
	for (size_t i = 0; i < NBVAL_TradeGrids; i++)
	{
		m_fake_grid[i].ClearHighlight();
	}

	//getting hovered cell's index
	int hovered_index = 0;
	for (size_t i = 0; i < NBVAL_TradeGrids; i++)
	{
		hovered_index = m_fake_grid[i].isCursorColliding(m_cursor);
		if (hovered_index >= 0)
		{
			m_focused_grid = i;
			break;
		}
	}

	//case: no cell is hovered
	if (hovered_index < 0)
	{
		m_focused_grid = 0;//identify which grid is focused
		m_focused_cell_index.x = -1;
		m_focused_cell_index.y = -1;
		m_focused_item = NULL;

		m_cursor.setAnimationLine(Cursor_NormalState);

		//destroy stats panel
		if (m_item_stats_panel)
		{
			delete m_item_stats_panel;
			m_item_stats_panel = NULL;
		}

		return m_focused_item;
	}
	//case: a cell is hovered
	else
	{
		//update knowledge of focused index
		m_focused_cell_index.x = hovered_index / m_fake_grid[m_focused_grid].squares.y;
		m_focused_cell_index.y = hovered_index % m_fake_grid[m_focused_grid].squares.y;

		//highlight new focused cell
		m_fake_grid[m_focused_grid].SetCellHighlightState(hovered_index, Slot_HighlightState);

		//update focused item
		GameObject* previous_focused_item = m_focused_item;
		m_focused_item = m_grid[m_focused_grid].grid[m_focused_cell_index.x][m_focused_cell_index.y];

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
				FocusedItemStates item_state = this == m_playerShip->m_SFHudPanel ? (m_focused_grid == 1 ? FocusedItem_Desequip : FocusedItem_Equip) : (m_focused_grid == Trade_ShopGrid ? FocusedItem_Buy : (m_focused_grid == Trade_EquippedGrid ? FocusedItem_SellOrDesequip : FocusedItem_SellOrEquip));
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state);
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

GameObject* SFTradePanel::GetCursor()
{
	return &m_cursor;
}

GameObject* SFTradePanel::GetFocusedItem()
{
	return m_focused_item;
}

void SFTradePanel::SetFocusedItem(GameObject* item)
{
	m_focused_item = item;
}

sf::Vector2i SFTradePanel::GetFocusedIndex()
{
	return m_focused_cell_index;
}

int SFTradePanel::GetFocusedIntIndex()
{
	return m_focused_cell_index.y + (m_focused_cell_index.x * m_grid[m_focused_grid].squares.y);
}

int SFTradePanel::GetFocusedGrid()
{
	return m_focused_grid;
}

ObjectGrid* SFTradePanel::GetGrid(bool fake_grid, int grid)
{
	if (fake_grid)
	{
		return &m_fake_grid[grid];
	}
	else
	{
		return &m_grid[grid];
	}
}

SFItemStatsPanel* SFTradePanel::GetItemStatsPanel()
{
	return m_item_stats_panel;
}

void SFTradePanel::SetItemStatsPanel(SFItemStatsPanel* panel)
{
	m_item_stats_panel = panel;
}

void SFTradePanel::ClearHighlight()
{
	for (size_t i = 0; i < NBVAL_TradeGrids; i++)
	{
		m_fake_grid[i].ClearHighlight();
	}
}

int SFTradePanel::GetItemsStatsPanelIndex()
{
	if (m_item_stats_panel)
	{
		return m_item_stats_panel->m_selected_option_index;
	}
	else
	{
		return -1;
	}
}

void SFTradePanel::SetItemsStatsPanelIndex(int index)
{
	if (m_item_stats_panel)
	{
		m_item_stats_panel->m_selected_option_index = index;
	}
}

int SFTradePanel::GetItemsStatsPanelNumberOfOptions()
{
	if (m_item_stats_panel)
	{
		return m_item_stats_panel->m_number_of_options;
	}
	else
	{
		return -1;
	}
}