#include "SFItemStatsPanel.h"

extern Game* CurrentGame;

//ITEMS STATS PANEL
SFItemStatsPanel::SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, FocusedItemStates item_state, GameObject* comparison_object) : SFPanel(size, SFPanel_ItemStats)
{
	if (object)
	{
		setSize(size);
		setOrigin(size.x / 2, size.y / 2);
		setFillColor(sf::Color(20, 20, 20, 230));//dark grey
		setOutlineThickness(0);
		m_comparison_object = comparison_object;

		if (comparison_object == NULL)
		{
			setPosition(object->getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X, object->getPosition().y + size.y / 2 + ITEM_STATS_SHOP_OFFSET_Y);
			
			if (getPosition().x + size.x / 2 + ITEM_STATS_SHOP_OFFSET_X > (*CurrentGame).m_screen_size.x)//clamp to screen border
				setPosition(sf::Vector2f((*CurrentGame).m_screen_size.x - size.x / 2 - ITEM_STATS_SHOP_OFFSET_X, getPosition().y));
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
				
				if (getPosition().x + size.x / 2 + ITEM_STATS_COMPARE_OFFSET_X + size.x + ITEM_STATS_SHOP_OFFSET_X > (*CurrentGame).m_screen_size.x)//clamp to screen border
					setPosition(sf::Vector2f((*CurrentGame).m_screen_size.x - size.x / 2 - ITEM_STATS_SHOP_OFFSET_X - size.x - ITEM_STATS_COMPARE_OFFSET_X, getPosition().y));
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
		
		if (comparison_object != NULL)
		{
			int equip_type = object->m_equipment_loot ? object->m_equipment_loot->m_equipmentType : (object->m_weapon_loot ? NBVAL_Equipment : -1);
			if (playerShip->m_SFHudPanel->GetGrid_v2(Trade_EquippedGrid)->m_elements[equip_type]->m_object != NULL)
			{
				m_title_text_comparison.setString("EQUIPPED");
				text_height += m_title_text_comparison.getGlobalBounds().height / 2;
				m_title_text_comparison.setPosition(getPosition().x - m_title_text_comparison.getGlobalBounds().width/2, getPosition().y - getSize().y / 2 + text_height);
				text_height += INTERACTION_INTERLINE + m_title_text_comparison.getGlobalBounds().height;

				DisplayItemStats(comparison_object);
			}
		}
		else
		{
			if (item_state != FocusedItem_Desequip)
			{
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
			}

			DisplayItemStats(object);
		}

		text_height += m_title_text.getGlobalBounds().height / 2;
		m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		text_height += m_title_text.getCharacterSize() + INTERACTION_INTERLINE;
		m_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

		if (comparison_object != NULL)
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

		if (m_comparison_object != NULL)
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
				ss_stats << "\nHyperspeed fuel: " << obj->m_hyperspeed_fuel;// << "\nContact damage: " << obj->m_damage;
				
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
				
					if (obj->m_bots.front()->m_weapon->m_ammunition->m_isBeam == false)
						ss_stats << "\nDPS: " << (floor)(1.f / obj->m_bots.front()->m_weapon->m_rate_of_fire * 100) / 100 * obj->m_bots.front()->m_weapon->m_multishot * obj->m_bots.front()->m_weapon->m_ammunition->m_damage * obj->m_bots.size();
					else
						ss_stats << "\nDPS: " << (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 * obj->m_bots.front()->m_weapon->m_multishot * obj->m_bots.front()->m_weapon->m_ammunition->m_damage * obj->m_bots.size();

					ss_stats << "\nDamage: " << obj->m_bots.front()->m_weapon->m_ammunition->m_damage;
					
					if (obj->m_bots.front()->m_weapon->m_rafale >= 0 && obj->m_bots.front()->m_weapon->m_ammunition->m_is_missile_model == false)
						ss_stats << "\nAmmo speed: " << obj->m_bots.front()->m_weapon->m_ammunition->m_speed.y;
					else if (obj->m_bots.front()->m_weapon->m_ammunition->m_is_missile_model == true)
						ss_stats << "\nGuided missiles";
					else 
						ss_stats << "\nContinuous beam";

					ss_stats.precision(1);

					if (obj->m_bots.front()->m_weapon->m_ammunition->m_isBeam == false)
						ss_stats << "\nFire rate: " << (floor)(1.f / obj->m_bots.front()->m_weapon->m_rate_of_fire * 100) / 100 << " shots/sec";
					else
						ss_stats << "\nFire rate: " << (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK);

					ss_stats.precision(0);

					if (obj->m_bots.front()->m_weapon->m_multishot > 1)
						ss_stats << "\nMultishot: " << obj->m_bots.front()->m_weapon->m_multishot << "\nDispersion: " << obj->m_bots.front()->m_weapon->m_dispersion << "°";
					else
						ss_stats << "\nSingle shot";

					if (obj->m_bots.front()->m_weapon->m_rafale > 0)
						ss_stats << "\nRafale: " << obj->m_bots.front()->m_weapon->m_rafale << " (cooldown: " << obj->m_bots.front()->m_weapon->m_rafale_cooldown << " sec";

					if (obj->m_bots.front()->m_pattern.m_pattern_type == Circle_ && obj->m_bots.front()->m_pattern.m_width > 0 && obj->m_bots.front()->m_pattern.m_height > 0)
						ss_stats << "\nPattern: Circle";
					else
						ss_stats << "\nPattern: Oscillator";
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

					if (obj->m_bots.front()->m_weapon->m_ammunition->m_pattern.m_pattern_type == Circle_)
						ss_stats << "\nEpic ability: waving trajectory";
				}
				else
					ss_stats << "\nNo effect";

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

					if (obj->m_ammunition->m_isBeam == false)
						ss_stats << "DPS: " << (floor)(1.f / obj->m_rate_of_fire * 100) / 100 * obj->m_multishot * obj->m_ammunition->m_damage;
					else
						ss_stats << "DPS: " << (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 * obj->m_multishot * obj->m_ammunition->m_damage;
				//}
				ss_stats << "\nDamage: " << obj->m_ammunition->m_damage;

				if (obj->m_rafale >= 0 && obj->m_ammunition->m_is_missile_model == false)
					ss_stats << "\nAmmo speed: " << obj->m_ammunition->m_speed.y;
				else if (obj->m_ammunition->m_is_missile_model == true)
					ss_stats << "\nGuided missiles";
				else
					ss_stats << "\nContinuous beam";
				
				ss_stats.precision(1);
				if (obj->m_ammunition->m_isBeam == false)
					ss_stats << "\nFire rate: " << (floor)(1.f / obj->m_rate_of_fire * 100) / 100 << " shots/sec";
				else
					ss_stats << "\nFire rate: " << (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 << " shots/sec";
				ss_stats.precision(0);

				if (obj->m_multishot > 1)
					ss_stats << "\nMultishot: " << obj->m_multishot << "\nDispersion: " << obj->m_dispersion << "°";
				else
					ss_stats << "\nSingle shot";

				if (obj->m_rafale > 0)
					ss_stats << "\nRafale: " << obj->m_rafale << " (cooldown: " << obj->m_rafale_cooldown << " sec";
				
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

				if (obj->m_ammunition->m_pattern.m_pattern_type == Circle_)
				{
					ss_stats << "\nEpic ability: waving trajectory";
				}

				if (obj->m_target_homing != NO_HOMING)
				{
					switch (obj->m_target_homing)
					{
						case HOMING:
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

				ss_stats << "\n\nLevel: " << obj->m_level << " (+" << obj->m_credits << " XP" << ". Quality: " << (int)obj->m_quality << "%)";
				ss_stats << "\nMoney value: $" << GameObject::GetPrice(obj->m_credits, obj->m_quality);
				break;
			}
		}

		m_title_text.setString(ss_itam_name.str());
		m_text.setString(ss_stats.str());
	}
}