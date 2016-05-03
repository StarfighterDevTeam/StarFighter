#include "SFInventoryPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, FocusedItemStates item_state, bool comparison) : SFPanel(size, SFPanel_ItemStats)
{
	if (object)
	{
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		setFillColor(sf::Color(20, 20, 20, 230));//dark grey
		setOutlineThickness(0);
		m_comparison = comparison;

		if (!comparison)
		{
			setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);
		}
		else
		{
			if (item_state == FocusedItem_Buy || item_state == FocusedItem_SellOrEquip)
			{
				setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X + size.x + ITEM_STATS_COMPARE_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);
			}
			else if (item_state == FocusedItem_Equip)
			{
				setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X - size.x - ITEM_STATS_COMPARE_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);
			}

			m_overblock.setSize(this->getSize());
			m_overblock.setOrigin(this->getOrigin());
			m_overblock.setPosition(this->getPosition());
			m_overblock.setFillColor(sf::Color(0, 0, 0, GHOST_ALPHA_VALUE));
		}
		
		m_selected_option_index = 0;

		//m_buttons[0] = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_BUTTON_A_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT),
		//	sf::Vector2f(INTERACTION_BUTTON_WIDTH / 2, INTERACTION_BUTTON_HEIGHT / 2));
		//m_buttons[1] = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_BUTTON_X_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT),
		//	sf::Vector2f(INTERACTION_BUTTON_WIDTH / 2, INTERACTION_BUTTON_HEIGHT / 2));
		//m_buttons[2] = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_BUTTON_Y_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT),
		//	sf::Vector2f(INTERACTION_BUTTON_WIDTH / 2, INTERACTION_BUTTON_HEIGHT / 2));
		//m_buttons[3] = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, INTERACTION_PANEL_MARGIN_TOP), sf::Vector2f(0, 0), INTERACTION_BUTTON_B_FILENAME, sf::Vector2f(INTERACTION_BUTTON_WIDTH, INTERACTION_BUTTON_HEIGHT),
		//	sf::Vector2f(INTERACTION_BUTTON_WIDTH / 2, INTERACTION_BUTTON_HEIGHT / 2));

		//texts
		m_title_text.setCharacterSize(18);
		m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		//m_options_text[0].setFont(*(*CurrentGame).m_font[Font_Arial]);
		//m_options_text[0].setCharacterSize(18);
		//m_options_text[1].setFont(*(*CurrentGame).m_font[Font_Arial]);
		//m_options_text[1].setCharacterSize(18);

		m_title_text_comparison.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_title_text_comparison.setCharacterSize(m_title_text.getCharacterSize());
		m_title_text_comparison.setColor(m_title_text.getColor());

		float text_height = 0;
		
		if (comparison)
		{
			int equip_type = object->m_equipment_loot ? object->m_equipment_loot->m_equipmentType : (object->m_weapon_loot ? NBVAL_Equipment : -1);
			GameObject* equipped_object = playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->grid[0][equip_type];
			if (equipped_object)
			{
				m_title_text_comparison.setString("EQUIPPED");
				text_height += m_title_text_comparison.getGlobalBounds().height / 2;
				m_title_text_comparison.setPosition(getPosition().x - m_title_text_comparison.getGlobalBounds().width/2, getPosition().y - getSize().y / 2 + text_height);
				text_height += INTERACTION_INTERLINE + m_title_text_comparison.getGlobalBounds().height;

				DisplayItemStats(equipped_object);
			}

			equipped_object = NULL;
		}
		else
		{
			//if (item_state != FocusedItem_Desequip)
			//{
				if (item_state == FocusedItem_Equip || item_state == FocusedItem_SellOrEquip)
				{
					m_title_text_comparison.setString("INVENTORY");
				}
				else if (item_state == FocusedItem_Buy)
				{
					m_title_text_comparison.setString("SHOP");
				}
				else
				{
					m_title_text_comparison.setString("EQUIPPED");
				}
				text_height += m_title_text_comparison.getGlobalBounds().height / 2;
				m_title_text_comparison.setPosition(getPosition().x - m_title_text_comparison.getGlobalBounds().width / 2, getPosition().y - getSize().y / 2 + text_height);
				text_height += INTERACTION_INTERLINE + m_title_text_comparison.getGlobalBounds().height;
			//}

			DisplayItemStats(object);
		}

		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		if (comparison)
		{
			return;
		}

		//Action texts
		m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);

		//BUY
		if (item_state == FocusedItem_Buy)
		{
			int price = object->m_equipment_loot ? GameObject::GetPrice(object->m_equipment_loot->m_credits, object->m_equipment_loot->m_quality) : GameObject::GetPrice(object->m_weapon_loot->m_credits, object->m_weapon_loot->m_quality);
			
			ostringstream ss_buy_and_equip;
			ss_buy_and_equip << "Buy and equip: $" << price;
			if (playerShip->m_money < price)
			{
				ss_buy_and_equip << " (insufficient credits)";
			}
			m_actions->SetString(ss_buy_and_equip.str(), ActionButton_A);

			ostringstream ss_buy;
			ss_buy << "Buy: $" << price;
			if (playerShip->m_money < price)
			{
				ss_buy << " (insufficient credits)";
			}
			m_actions->SetString(ss_buy.str(), ActionButton_X);

			if (playerShip->m_money < price)
			{
				m_actions->m_texts[ActionButton_A].setColor(sf::Color(255, 50, 50, 255));//red
				m_actions->m_texts[ActionButton_X].setColor(sf::Color(255, 50, 50, 255));//red
			}
		}
		//SELL
		else if (item_state == FocusedItem_SellOrEquip || item_state == FocusedItem_SellOrDesequip)
		{
			int price = object->m_equipment_loot ? GameObject::GetPrice(object->m_equipment_loot->m_credits, object->m_equipment_loot->m_quality) : GameObject::GetPrice(object->m_weapon_loot->m_credits, object->m_weapon_loot->m_quality);
			ostringstream ss_sell;
			ss_sell << "Sell: $" << price;
			//m_options_text[0].setString(ss_sell.str());
			m_actions->SetString(ss_sell.str(), ActionButton_A);

			//SELL / EQUIP
			if (item_state == FocusedItem_SellOrEquip)
			{
				//m_options_text[1].setString("Equip");
				m_actions->SetString("Equip", ActionButton_X);
			}
			//SELL / DESEQUIP
			else if (item_state == FocusedItem_SellOrDesequip)
			{
				//m_options_text[1].setString("Desequip");
				m_actions->SetString("Desequip", ActionButton_X);
			}
		}
		//EQUIP
		else if (item_state == FocusedItem_Equip)
		{
			//m_options_text[0].setString("Equip");
			m_actions->SetString("Equip", ActionButton_A);
			m_actions->SetString("(hold) Throw", ActionButton_X);
		}
		//DESEQUIP
		else if (item_state == FocusedItem_Desequip)
		{
			//m_options_text[0].setString("Desequip");
			m_actions->SetString("Desequip", ActionButton_A);
			m_actions->SetString("(hold) Throw", ActionButton_X);
		}

		//options texts
		text_height += INTERACTION_INTERBLOCK + m_text.getGlobalBounds().height;
		m_actions->SetPosition(sf::Vector2f(getPosition().x - (getSize().x / 2) + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	}
}

SFItemStatsPanel::~SFItemStatsPanel()
{

}

void SFItemStatsPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		screen.draw(m_title_text);
		if (!m_title_text_comparison.getString().isEmpty())
		{
			screen.draw(m_title_text_comparison);
		}
		screen.draw(m_text);
		
		if (m_actions)
		{
			m_actions->Draw(screen);
		}

		if (m_comparison)
		{
			screen.draw(m_overblock);
		}

		//for (size_t i = 0; i < 2; i++)
		//{
		//	screen.draw(m_options_text[i]);
		//}
		//for (size_t i = 0; i < m_number_of_options; i++)
		//{
		//	screen.draw(m_options_text[i]);
		//	screen.draw(m_buttons[i]);
		//}
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
				ss_stats.precision(2);
				ss_stats << "Hyperspeed: " << obj->m_hyperspeed;
				ss_stats.precision(0);
				ss_stats << "\nHyperspeed fuel: " << obj->m_hyperspeed_fuel << "\nContact damage: " << obj->m_damage;
				
				if (obj->m_can_hyperspeed)
				{
					ss_stats << "\n\nUnique ability: cruise (hold Y or LCtrl)";
				}
				if (obj->m_can_jump)
				{
					ss_stats << "\n\nUnique ability: warp (press Y or LCtrl + direction)";
				}

				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
				break;
			}
			case Armor:
			{
				Equipment* obj = object->m_equipment_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "HULL: " << standard_name;
				ss_stats << "Hull pts: " << obj->m_armor;
				ss_stats << "\n\nBombs: " << obj->m_bombs << " (press B or LShift, requires Fuel)";
				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
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
				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
				break;
			}
			case Module:
			{
				Equipment* obj = object->m_equipment_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "MODULE: " << standard_name;
				if (!obj->m_bots.empty())
				{
					ss_stats << "Adding " << obj->m_bots.size() << " drone. Drone stats:";
					//if (obj->m_bots.front()->m_weapon->m_shot_mode != NoShotMode)
					//{
					//	ss_stats << "\nDPS: " << (floor)(1 / obj->m_bots.front()->m_weapon->m_rate_of_fire * 100) / 100 * obj->m_bots.front()->m_weapon->m_ammunition->m_damage * obj->m_bots.size();
					//}
					//else
					//{
						ss_stats << "\nDPS: " << (floor)(1 / obj->m_bots.front()->m_weapon->m_rate_of_fire * 100) / 100 * obj->m_bots.front()->m_weapon->m_multishot * obj->m_bots.front()->m_weapon->m_ammunition->m_damage * obj->m_bots.size();
					//}

					ss_stats << "\nDamage: " << obj->m_bots.front()->m_weapon->m_ammunition->m_damage;
					ss_stats << "\nAmmo speed: " << obj->m_bots.front()->m_weapon->m_ammunition->m_speed.y;
					ss_stats.precision(1);
					ss_stats << "\nFire rate: " << (floor)(1 / obj->m_bots.front()->m_weapon->m_rate_of_fire * 100) / 100 << " shots/sec";
					ss_stats.precision(0);

					if (obj->m_bots.front()->m_weapon->m_multishot > 1)
					{
						//ss_stats << "\nMultishot: " << obj->m_bots.front()->m_weapon->m_multishot << "\nSpread: " << obj->m_bots.front()->m_weapon->m_xspread << "\nDispersion: " << obj->m_bots.front()->m_weapon->m_dispersion << "°";
						ss_stats << "\nMultishot: " << obj->m_bots.front()->m_weapon->m_multishot << "\nDispersion: " << obj->m_bots.front()->m_weapon->m_dispersion << "°";
					}
					else
					{
						ss_stats << "\nSingle shot";
					}
					if (obj->m_bots.front()->m_weapon->m_rafale > 0)
					{
						ss_stats << "\nRafale: " << obj->m_bots.front()->m_weapon->m_rafale << " (cooldown: " << obj->m_bots.front()->m_weapon->m_rafale_cooldown << " sec";
					}

					//ss_stats << "\nFiring style: ";
					//switch (obj->m_bots.front()->m_weapon->m_shot_mode)
					//{
					//	case NoShotMode:
					//	{
					//		ss_stats << "Parallel shots";
					//		break;
					//	}
					//	case AlternateShotMode:
					//	{
					//		ss_stats << "Alternating shots";
					//		break;
					//	}
					//	case AscendingShotMode:
					//	{
					//		ss_stats << "Ascending shots";
					//		break;
					//	}
					//	case DescendingShotMode:
					//	{
					//		ss_stats << "Descending shots";
					//		break;
					//	}
					//}

					if (obj->m_bots.front()->m_weapon->m_target_homing != NO_HOMING)
					{
						switch (obj->m_bots.front()->m_weapon->m_target_homing)
						{
							case HOMING:
							case SUPER_HOMING:
							{
								ss_stats << "\nEpic ability: target homing";
								break;
							}
							case SEMI_HOMING:
							{
								ss_stats << "\nEpic ability: target homing per salvo";
								break;
							}
						}
					}

					if (obj->m_bots.front()->m_weapon->m_ammunition->m_Pattern.m_currentPattern == Oscillator)
					{
						ss_stats << "\nEpic ability: waving trajectory";
					}
				}
				else
				{
					ss_stats << "\nNo effect";
				}
				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
				break;
			}
			case NBVAL_Equipment:
			{
				Weapon* obj = object->m_weapon_loot;
				string standard_name = ReplaceAll(obj->m_display_name, "_", " ");
				ss_itam_name << "MAIN WEAPON: " << standard_name;
				//if (obj->m_shot_mode != NoShotMode)
				//{
				//	ss_stats.precision(0);
				//	ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_ammunition->m_damage;
				//}
				//else
				//{
					ss_stats.precision(0);
					ss_stats << "DPS: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 * obj->m_multishot * obj->m_ammunition->m_damage;
				//}
				ss_stats << "\nDamage: " << obj->m_ammunition->m_damage;
				ss_stats << "\nAmmo speed: " << obj->m_ammunition->m_speed.y;
				ss_stats.precision(1);
				ss_stats << "\nFire rate: " << (floor)(1 / obj->m_rate_of_fire * 100) / 100 << " shots/sec";
				ss_stats.precision(0);

				if (obj->m_multishot > 1)
				{
					//ss_stats << "\nMultishot: " << obj->m_multishot << "\nSpread: " << obj->m_xspread << "\nDispersion: " << obj->m_dispersion << "°";
					ss_stats << "\nMultishot: " << obj->m_multishot << "\nDispersion: " << obj->m_dispersion << "°";
				}
				else
				{
					ss_stats << "\nSingle shot";
				}
				if (obj->m_rafale > 0)
				{
					ss_stats << "\nRafale: " << obj->m_rafale << " (cooldown: " << obj->m_rafale_cooldown << " sec";
				}
				
				//ss_stats << "\nFiring style: ";
				//switch (obj->m_shot_mode)
				//{
				//	case NoShotMode:
				//	{
				//		ss_stats << "Parallel shots";
				//		break;
				//	}
				//	case AlternateShotMode:
				//	{
				//		ss_stats << "Alternating shots";
				//		break;
				//	}
				//	case AscendingShotMode:
				//	{
				//		ss_stats << "Ascending shots";
				//		break;
				//	}
				//	case DescendingShotMode:
				//	{
				//		ss_stats << "Descending shots";
				//		break;
				//	}
				//}

				if (obj->m_target_homing != NO_HOMING)
				{
					switch (obj->m_target_homing)
					{
						case HOMING:
						case SUPER_HOMING:
						{
							ss_stats << "\nEpic ability: target homing";
							break;
						}
						case SEMI_HOMING:
						{
							ss_stats << "\nEpic ability: target homing per salvo";
							break;
						}
					}
				}

				if (obj->m_ammunition->m_Pattern.m_currentPattern == Oscillator)
				{
					ss_stats << "\nEpic ability: waving trajectory";
				}

				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
				break;
			}
		}

		m_title_text.setString(ss_itam_name.str());
		m_text.setString(ss_stats.str());
	}
}

//INVENTORY PANEL
SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip, SFPanelTypes panel_type) : SFPanel(size, panel_type)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_item_stats_panel_compare = NULL;
	m_focused_grid = 0;
	m_has_prioritary_feedback = false;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));
	m_cursor.m_visible = true;

	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		m_grid[i] = NULL;
		m_fake_grid[i] = NULL;
		m_quality_grid[i] = NULL;
		m_grey_grid[i] = NULL;
	}

	//size and position
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));
}

SFInventoryPanel::~SFInventoryPanel()
{
	if (m_item_stats_panel)
	{
		delete m_item_stats_panel;
		m_item_stats_panel = NULL;
	}
	if (m_item_stats_panel_compare)
	{
		delete m_item_stats_panel_compare;
		m_item_stats_panel_compare = NULL;
	}

	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_grid[i])
		{
			delete m_grid[i];
			m_grid[i] = NULL;
		}
		if (m_fake_grid[i])
		{
			delete m_fake_grid[i];
			m_fake_grid[i] = NULL;
		}
		if (m_quality_grid[i])
		{
			delete m_quality_grid[i];
			m_quality_grid[i] = NULL;
		}
		if (m_grey_grid[i])
		{
			delete m_grey_grid[i];
			m_grey_grid[i] = NULL;
		}
	}
}

GameObject* SFInventoryPanel::GetEquivalentEquippedItem(Ship* playerShip, GameObject* item)
{
	if (m_focused_grid == Trade_EquippedGrid)
	{
		return NULL;
	}

	int equip_type = item->m_equipment_loot ? item->m_equipment_loot->m_equipmentType : (item->m_weapon_loot ? NBVAL_Equipment : -1);
	if (playerShip && playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->grid[0][equip_type])
	{
		return playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->grid[0][equip_type];
	}
	else
	{
		return NULL;
	}
}

void SFInventoryPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	//update background colors
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_fake_grid[i])
		{
			UpdateBackgroundColors(m_quality_grid[i], m_grid[i]);
		}
	}

	//update hovered item and highlight feedbacks
	GetHoveredObjectInGrid();

	//cover cells with a semi-transparent black if player doesn't have the money required
	if (m_fake_grid[Trade_ShopGrid])
	{
		UpdateGreyMaskOnInsufficientCredits(m_grey_grid[Trade_ShopGrid], m_grid[Trade_ShopGrid], m_playerShip);
	}
}

void SFInventoryPanel::UpdateGreyMaskOnInsufficientCredits(ObjectGrid* grey_grid, ObjectGrid* grid, Ship* playerShip)
{
	if (!grey_grid || !playerShip || !grid)
	{
		return;
	}

	for (int i = 0; i < grey_grid->squares.x; i++)
	{
		for (int j = 0; j < grey_grid->squares.y; j++)
		{
			if (grid->grid[i][j])
			{
				if (grid->grid[i][j]->m_equipment_loot)
				{
					grey_grid->grid[i][j]->m_visible = playerShip->m_money < GameObject::GetPrice(grid->grid[i][j]->m_equipment_loot->m_credits, grid->grid[i][j]->m_equipment_loot->m_quality);
				}
				else if (grid->grid[i][j]->m_weapon_loot)
				{
					grey_grid->grid[i][j]->m_visible = playerShip->m_money < GameObject::GetPrice(grid->grid[i][j]->m_weapon_loot->m_credits, grid->grid[i][j]->m_weapon_loot->m_quality);
				}
			}
		}
	}
}

void SFInventoryPanel::SetGridSlotsVisibility(ObjectGrid* grid, bool visible)
{
	if (!grid)
	{
		return;
	}

	for (int i = 0; i < grid->squares.x; i++)
	{
		for (int j = 0; j < grid->squares.y; j++)
		{
			if (grid->grid[i][j])
			{
				grid->grid[i][j]->m_visible = visible;
			}
		}
	}
}


void SFInventoryPanel::UpdateBackgroundColors(ObjectGrid* color_grid, ObjectGrid* object_grid)
{
	if (!color_grid || !object_grid)
	{
		return;
	}

	for (int i = 0; i < color_grid->squares.x; i++)
	{
		for (int j = 0; j < color_grid->squares.y; j++)
		{
			if (color_grid->grid[i][j] && i < object_grid->squares.x && j < object_grid->squares.y)
			{
				if (object_grid->grid[i][j])
				{
					float quality = object_grid->grid[i][j]->m_equipment_loot ? object_grid->grid[i][j]->m_equipment_loot->m_quality : (object_grid->grid[i][j]->m_weapon_loot ? object_grid->grid[i][j]->m_weapon_loot->m_quality : -1);
					if (quality < 0)
					{
						color_grid->grid[i][j]->m_visible = false;
						printf("Error in SFInventoryPanel::UpdateBackgroundColors(): trying to get the quality of an item that doesn't have any equipment or weapon.\n");
					}
					else
					{
						color_grid->grid[i][j]->setAnimationLine((int)Game::GetItemQualityClass(quality));
						color_grid->grid[i][j]->m_visible = true;
					}
				}
				else
				{
					color_grid->grid[i][j]->m_visible = false;
				}
			}
			else if (color_grid->grid[i][j])
			{
				color_grid->grid[i][j]->m_visible = false;
			}
		}
	}
}

void SFInventoryPanel::DrawItemLevels(sf::RenderTexture& screen)
{
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_grid[i])
		{
			for (int j = 0; j < m_grid[i]->squares.x; j++)
			{
				for (int k = 0; k < m_grid[i]->squares.y; k++)
				{
					if (m_grid[i]->grid[j][k])
					{
						sf::Text item_level;
						item_level.setCharacterSize(11);

						if (m_cursor.m_visible)
						{
							item_level.setColor(sf::Color(255, 255, 255, 255));
						}
						else
						{
							item_level.setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE/2));
						}
						
						item_level.setFont(*(*CurrentGame).m_font[Font_Terminator]);
						int equip_type = m_grid[i]->grid[j][k]->m_equipment_loot ? m_grid[i]->grid[j][k]->m_equipment_loot->m_equipmentType : (m_grid[i]->grid[j][k]->m_weapon_loot ? NBVAL_Equipment : -1);
						ostringstream ss;
						if (equip_type == NBVAL_Equipment)
						{
							ss << m_grid[i]->grid[j][k]->m_weapon_loot->m_level;
						}
						else if (equip_type >= 0 && equip_type < NBVAL_Equipment)
						{
							ss << m_grid[i]->grid[j][k]->m_equipment_loot->m_level;
						}
						item_level.setString(ss.str());
						item_level.setPosition(sf::Vector2f(m_grid[i]->grid[j][k]->getPosition().x + m_grid[i]->grid[j][k]->m_size.x / 2 - item_level.getGlobalBounds().width *3/4, m_grid[i]->grid[j][k]->getPosition().y + m_grid[i]->grid[j][k]->m_size.y / 2 - item_level.getCharacterSize()*3/4));
						screen.draw(item_level);
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

		for (int i = 0; i < NBVAL_TradeGrids; i++)
		{
			if (m_fake_grid[i])
			{
				m_fake_grid[i]->Draw(screen);
			}
			if (m_quality_grid[i])
			{
				m_quality_grid[i]->Draw(screen);
			}
			if (m_grid[i])
			{
				m_grid[i]->Draw(screen);
			}
			if (m_grey_grid[i])
			{
				m_grey_grid[i]->Draw(screen);
			}
		}

		DrawItemLevels(screen);

		screen.draw(m_title_text);
		if (m_cursor.m_visible)
		{
			screen.draw(m_cursor);
		}
	
		if (m_actions)
		{
			m_actions->Draw(screen);
		}
	}
}

GameObject* SFInventoryPanel::GetHoveredObjectInGrid()
{
	//reset previous highlight
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_fake_grid[i])
		{
			m_fake_grid[i]->ClearHighlight();
		}

		if (i == Trade_EquippedGrid)
		{
			//reset empty slots feebdacks
			for (int j = 0; j < NBVAL_Equipment + 1; j++)
			{
				if (m_grid[Trade_EquippedGrid]->grid[0][j])
				{
					m_fake_grid[Trade_EquippedGrid]->grid[0][j]->setAnimationLine(Slot_NormalState);
				}
				else
				{
					m_fake_grid[Trade_EquippedGrid]->grid[0][j]->setAnimationLine(Slot_EngineFake + j);
				}
			}
		}
	}

	//getting hovered cell's index
	int hovered_index = 0;
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		hovered_index = m_fake_grid[i] ? m_fake_grid[i]->isCursorColliding(m_cursor) : -1;
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
		if (m_item_stats_panel_compare)
		{
			delete m_item_stats_panel_compare;
			m_item_stats_panel_compare = NULL;
		}

		return m_focused_item;
	}
	//case: a cell is hovered
	else
	{
		//update knowledge of focused index
		m_focused_cell_index.x = hovered_index / m_fake_grid[m_focused_grid]->squares.y;
		m_focused_cell_index.y = hovered_index % m_fake_grid[m_focused_grid]->squares.y;

		//highlight new focused cell
		m_fake_grid[m_focused_grid]->SetCellHighlightState(hovered_index, Slot_HighlightState);

		//update focused item
		GameObject* previous_focused_item = m_focused_item;
		m_focused_item = m_grid[m_focused_grid]->grid[m_focused_cell_index.x][m_focused_cell_index.y];

		//item hovered
		if (m_focused_item)
		{
			if (!m_has_prioritary_feedback)
			{
				m_cursor.setAnimationLine(Cursor_ActionState);
			}

			//update item stats panel
			if (previous_focused_item != m_focused_item)
			{
				delete m_item_stats_panel;
				if (m_item_stats_panel_compare)
				{
					delete m_item_stats_panel_compare;
					m_item_stats_panel_compare = NULL;
				}
			}
			if (!previous_focused_item || previous_focused_item != m_focused_item)
			{
				FocusedItemStates item_state = this == m_playerShip->m_SFHudPanel ? (m_focused_grid == Trade_EquippedGrid ? FocusedItem_Desequip : FocusedItem_Equip) : (m_focused_grid == Trade_ShopGrid ? FocusedItem_Buy : (m_focused_grid == Trade_EquippedGrid ? FocusedItem_SellOrDesequip : FocusedItem_SellOrEquip));
				m_item_stats_panel = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state, false);
				//update compare panel
				if (GetEquivalentEquippedItem(m_playerShip, m_focused_item))
				{
					m_item_stats_panel_compare = new SFItemStatsPanel(m_focused_item, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state, true);
				}
				else
				{
					if (m_item_stats_panel_compare)
					{
						delete m_item_stats_panel_compare;
						m_item_stats_panel_compare = NULL;
					}
				}
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
			if (m_item_stats_panel_compare)
			{
				delete m_item_stats_panel_compare;
				m_item_stats_panel_compare = NULL;
			}
		}

		previous_focused_item = NULL;
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
	if (fake_grid)
	{
		return m_fake_grid[grid];
	}
	else
	{
		return m_grid[grid];
	}
}

void SFInventoryPanel::ClearHighlight()
{
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_fake_grid[i])
		{
			m_fake_grid[i]->ClearHighlight();
		}
	}
}

sf::Vector2i SFInventoryPanel::GetFocusedIndex()
{
	return m_focused_cell_index;
}

int SFInventoryPanel::GetFocusedIntIndex()
{
	return m_grid[m_focused_grid] ? m_focused_cell_index.y + (m_focused_cell_index.x * m_grid[m_focused_grid]->squares.y) : -1;
}

int SFInventoryPanel::GetFocusedGrid()
{
	return m_focused_grid;
}

int SFInventoryPanel::GetItemsStatsPanelIndex()
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

void SFInventoryPanel::SetItemsStatsPanelIndex(int index)
{
	if (m_item_stats_panel)
	{
		m_item_stats_panel->m_selected_option_index = index;
	}
}

//HUD PANEL
SFHUDPanel::SFHUDPanel(sf::Vector2f size, Ship* playerShip) : SFInventoryPanel(size, playerShip, SFPanel_HUD)
{
	setOrigin(0, 0);
	setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	setOutlineThickness(0);
	setPosition(SCENE_SIZE_X, 0);
	m_cursor.m_visible = false;

	m_fake_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), true);
	m_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false);
	m_quality_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false, false, true);
	m_grey_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false, true, false);
	SetGridSlotsVisibility(m_grey_grid[Trade_StashGrid], true);

	m_fake_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), true);
	m_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false);
	m_quality_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false, false, true);
	m_grey_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false, true, false);
	SetGridSlotsVisibility(m_grey_grid[Trade_EquippedGrid], true);

	m_fake_grid[Trade_EquippedGrid]->grid[0][Engine]->setAnimationLine(Slot_EngineFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Armor]->setAnimationLine(Slot_ArmorFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Shield]->setAnimationLine(Slot_ShieldFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Module]->setAnimationLine(Slot_ModuleFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][NBVAL_Equipment]->setAnimationLine(Slot_WeaponFake);

	if (playerShip)
	{
		//int gauges and texts (upper part)
		m_armorBar.setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
		m_armorBar.setOrigin(0, 0);

		m_shieldBar.setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
		m_shieldBar.setOrigin(0, 0);

		m_fuelBar.setFillColor(sf::Color(COLOR_YELLOW_R_VALUE, COLOR_YELLOW_G_VALUE, COLOR_YELLOW_B_VALUE, COLOR_YELLOW_A_VALUE));//yellow
		m_fuelBar.setOrigin(0, 0);

		m_fuelBarOverblock.setFillColor(sf::Color(0, 0, 0, GHOST_ALPHA_VALUE));//black
		m_fuelBarOverblock.setOrigin(0, 0);
		m_fuelBarOverblock.setOutlineThickness(0);

		m_armorBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
		m_armorBarContainer.setOutlineThickness(1);
		m_armorBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_armorBarContainer.setOrigin(0, 0);

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

		//lower part
		m_money_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_money_text.setCharacterSize(20);
		m_money_text.setColor(_white);

		m_graze_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_graze_text.setCharacterSize(14);
		m_graze_text.setColor(_white);

		m_hazardscore_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hazardscore_text.setCharacterSize(14);
		m_hazardscore_text.setColor(_white);

		m_hitstaken_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_hitstaken_text.setCharacterSize(14);
		m_hitstaken_text.setColor(_white);

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

		m_equipment_title.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_equipment_title.setCharacterSize(14);
		m_equipment_title.setColor(sf::Color::White);
		m_equipment_title.setString("EQUIPMENT");

		m_inventory_title.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_inventory_title.setCharacterSize(14);
		m_inventory_title.setColor(sf::Color::White);
		m_inventory_title.setString("INVENTORY");

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
		m_fuelBarOverblock.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		
		text_height += 10 + ARMOR_BAR_SIZE_Y;
		m_xpBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_INTERBLOCK + ARMOR_BAR_SIZE_Y;
		m_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		//grids
		text_height = GRID_POSITION_Y;
		m_equipment_title.setPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		text_height += m_equipment_title.getCharacterSize() + INTERACTION_INTERLINE*2;

		m_fake_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_quality_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_grey_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
	
		text_height += INTERACTION_INTERBLOCK + m_fake_grid[Trade_EquippedGrid]->squares.x*GRID_SLOT_SIZE;
		m_inventory_title.setPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		text_height += m_inventory_title.getCharacterSize() + INTERACTION_INTERLINE * 2;

		m_fake_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_quality_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		m_grey_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));

		//lower part
		text_height += INTERACTION_INTERBLOCK*2 + m_fake_grid[Trade_StashGrid]->squares.x*GRID_SLOT_SIZE;
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
		m_hitstaken_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_hitstaken_text.getCharacterSize();
		m_beastscore_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_beastscore_text.getCharacterSize();
		m_framerate_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
	}
	
	//init cursor position on first cell
	if (m_fake_grid[Trade_EquippedGrid] && m_fake_grid[Trade_EquippedGrid]->grid[0][0])
	{
		m_cursor.setPosition(m_fake_grid[Trade_EquippedGrid]->grid[0][0]->getPosition().x, m_fake_grid[Trade_EquippedGrid]->grid[0][0]->getPosition().y);
	}
}

void SFHUDPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	SFInventoryPanel::Update(deltaTime, inputs_directions);

	if (m_cursor.m_visible)
	{
		m_equipment_title.setColor(sf::Color(255, 255, 255, 255));
		m_inventory_title.setColor(sf::Color(255, 255, 255, 255));
	}
	else
	{
		m_equipment_title.setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE));
		m_inventory_title.setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE));
	}
	

	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_grey_grid[i])
		{
			m_grey_grid[i]->m_visible = !m_cursor.m_visible;
		}
	}

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
		m_fuelBarOverblock.setSize(sf::Vector2f(1, FUEL_BAR_SIZE_Y));
	}
	else
	{
		m_fuelBar.setSize(sf::Vector2f(1 + (1.0f * m_playerShip->m_hyperspeed_fuel / m_playerShip->m_hyperspeed_fuel_max * ARMOR_BAR_SIZE_X), FUEL_BAR_SIZE_Y));
		m_fuelBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, FUEL_BAR_SIZE_Y));
		if (m_playerShip->m_hyperspeed_fuel > m_playerShip->m_hyperspeed_fuel_max / BOMB_DEFAULT_NUMBER)
		{
			m_fuelBarOverblock.setSize(sf::Vector2f(1.0f * m_playerShip->m_hyperspeed_fuel_max / BOMB_DEFAULT_NUMBER / m_playerShip->m_hyperspeed_fuel_max * ARMOR_BAR_SIZE_X, FUEL_BAR_SIZE_Y));
			m_fuelBarOverblock.setPosition(sf::Vector2f(m_fuelBar.getPosition().x + m_fuelBar.getSize().x - m_fuelBarOverblock.getSize().x, m_fuelBar.getPosition().y));
		}
		else
		{
			m_fuelBarOverblock.setSize(sf::Vector2f(1, FUEL_BAR_SIZE_Y));
		}
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

	//score destruction
	ostringstream ss_sc;
	ss_sc << (*CurrentGame).m_hazard << " / " << (*CurrentGame).m_hazardSpawned;
	m_hazardscore_text.setString("Destruction: " + ss_sc.str());

	//score hits taken
	ostringstream ss_ht;
	ss_ht << m_playerShip->m_hits_taken;
	m_hitstaken_text.setString("Hits taken: " + ss_ht.str());

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

	screen.draw(m_equipment_title);
	screen.draw(m_inventory_title);

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
		screen.draw(m_fuelBarOverblock);
		screen.draw(m_fuel_text);
	}

	screen.draw(m_money_text);
	screen.draw(m_graze_text);
	screen.draw(m_hazardscore_text);
	screen.draw(m_hitstaken_text);
	screen.draw(m_beastscore_text);
	screen.draw(m_xpBar);
	screen.draw(m_level_text);
	screen.draw(m_scene_text);
	screen.draw(m_framerate_text);
	screen.draw(m_text);

	if (m_item_stats_panel)
	{
		m_item_stats_panel->Draw(screen);
	}
	if (m_item_stats_panel_compare)
	{
		m_item_stats_panel_compare->Draw(screen);
	}
}

//TRADE PANEL
SFTradePanel::SFTradePanel(sf::Vector2f size, Ship* playerShip) : SFInventoryPanel(size, playerShip, SFPanel_Trade)
{
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_item_stats_panel_compare = NULL;
	m_focused_grid = 0;

	m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	m_actions->SetString("Quit", ActionButton_B);

	m_title_text2.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_title_text2.setCharacterSize(m_title_text.getCharacterSize());
	m_title_text2.setColor(m_title_text.getColor());
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_fake_grid[Trade_ShopGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), true);
	m_grid[Trade_ShopGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false);
	m_quality_grid[Trade_ShopGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false, false, true);
	m_grey_grid[Trade_ShopGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHOP_GRID_NB_LINES, SHOP_GRID_NB_ROWS), false, true);

	m_fake_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), true);
	m_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false);
	m_quality_grid[Trade_StashGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false, false, true);

	m_fake_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), true);
	m_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false);
	m_quality_grid[Trade_EquippedGrid] = new ObjectGrid(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false, false, true);

	m_fake_grid[Trade_EquippedGrid]->grid[0][Engine]->setAnimationLine(Slot_EngineFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Armor]->setAnimationLine(Slot_ArmorFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Shield]->setAnimationLine(Slot_ShieldFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][Module]->setAnimationLine(Slot_ModuleFake);
	m_fake_grid[Trade_EquippedGrid]->grid[0][NBVAL_Equipment]->setAnimationLine(Slot_WeaponFake);

	//texts
	if (playerShip)
	{
		//player name and content
		m_title_text2.setString(m_playerShip->m_display_name);
		//copy player grids into trade panel
		for (size_t i = 0; i < m_grid[Trade_EquippedGrid]->squares.x; i++)
		{
			for (size_t j = 0; j < m_grid[Trade_EquippedGrid]->squares.y; j++)
			{
				if (i < playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->squares.x && j < playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->squares.y && playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->grid[i][j])
				{
					m_grid[Trade_EquippedGrid]->grid[i][j] = playerShip->m_SFHudPanel->GetGrid(false, Trade_EquippedGrid)->grid[i][j]->Clone();
				}
			}
		}
		for (size_t i = 0; i < m_grid[Trade_StashGrid]->squares.x; i++)
		{
			for (size_t j = 0; j < m_grid[Trade_StashGrid]->squares.y; j++)
			{
				if (i < playerShip->m_SFHudPanel->GetGrid(false, Trade_StashGrid)->squares.x && j < playerShip->m_SFHudPanel->GetGrid(false, Trade_StashGrid)->squares.y && playerShip->m_SFHudPanel->GetGrid(false, Trade_StashGrid)->grid[i][j])
				{
					m_grid[Trade_StashGrid]->grid[i][j] = playerShip->m_SFHudPanel->GetGrid(false, Trade_StashGrid)->grid[i][j]->Clone();
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
					m_grid[Trade_ShopGrid]->insertObject(*m_playerShip->m_targetShop->m_items[i], i, true);
				}
			}
		}
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

	m_fake_grid[Trade_ShopGrid]->SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grid[Trade_ShopGrid]->SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_quality_grid[Trade_ShopGrid]->SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grey_grid[Trade_ShopGrid]->SetGridPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	m_fake_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));
	m_quality_grid[Trade_EquippedGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	float text_height2 = text_height += INTERACTION_INTERBLOCK + m_fake_grid[Trade_EquippedGrid]->grid[0][0]->m_size.y;
	m_fake_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));
	m_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));
	m_quality_grid[Trade_StashGrid]->SetGridPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height2));
	
	//quit button
	text_height += m_fake_grid[Trade_ShopGrid]->squares.x * GRID_SLOT_SIZE;
	text_height -= INTERACTION_INTERBLOCK;
	text_height -= INTERACTION_INTERBLOCK;
	m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//init cursor position on first cell
	m_cursor.setPosition(m_fake_grid[Trade_ShopGrid]->grid[0][0]->getPosition().x, m_fake_grid[Trade_ShopGrid]->grid[0][0]->getPosition().y);

	//build separator between shop items and player items
	m_separator.setSize(sf::Vector2f(1, size.y));
	m_separator.setOrigin(m_separator.getSize().x / 2, m_separator.getSize().y/2);
	m_separator.setPosition(getPosition());
	m_separator.setFillColor(getOutlineColor());
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

