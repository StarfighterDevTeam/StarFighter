#include "SFHUDPanel.h"

extern Game* CurrentGame;

//HUD PANEL
SFHUDPanel::SFHUDPanel(sf::Vector2f size, Ship* playerShip) : SFInventoryPanel(size, playerShip, SFPanel_HUD)
{
	setOrigin(0, 0);
	setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	setOutlineThickness(0);
	setPosition(SCENE_SIZE_X, 0);

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

		m_comboBar.setSize(sf::Vector2f((1.0f * playerShip->m_combo_count / playerShip->m_combo_count_max) * COMBO_BAR_SIZE_X, COMBO_BAR_SIZE_Y));
		m_comboBar.setFillColor(sf::Color(COLOR_LIGHT_BLUE_R_VALUE, COLOR_LIGHT_BLUE_G_VALUE, COLOR_LIGHT_BLUE_B_VALUE, COLOR_LIGHT_BLUE_A_VALUE));//light blue
		m_comboBar.setOrigin(0, 0);

		m_comboBarContainer.setSize(sf::Vector2f(COMBO_BAR_SIZE_X, COMBO_BAR_SIZE_Y));
		m_comboBarContainer.setFillColor(sf::Color(0, 0, 0, 255));//light blue
		m_comboBarContainer.setOutlineThickness(1);
		m_comboBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_comboBarContainer.setOrigin(0, 0);

		sf::Color _white = sf::Color::Color(255, 255, 255, 180);//semi-transparent white

		m_text.setColor(_white);

		m_combo_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_combo_text.setCharacterSize(18);
		m_combo_text.setColor(_white);
		
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

		text_height = GRID_POSITION_Y - COMBO_BAR_SIZE_Y - 2 * INTERACTION_INTERBLOCK;
		m_comboBar.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		m_comboBarContainer.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		m_combo_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + COMBO_BAR_SIZE_X + m_combo_text.getGlobalBounds().width / 2 + 8, text_height - 4);

		text_height = GRID_POSITION_Y - INTERACTION_INTERBLOCK;
		m_equipment_title.setPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		text_height += m_equipment_title.getCharacterSize() + INTERACTION_INTERLINE*2;

		sf::Vector2f position0 = sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
	
		text_height += INTERACTION_INTERBLOCK + 1 * GRID_SLOT_SIZE;
		m_inventory_title.setPosition(sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height));
		text_height += m_inventory_title.getCharacterSize() + INTERACTION_INTERLINE * 2;

		sf::Vector2f position1 = sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += STASH_GRID_NB_LINES * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
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

		//Create grids
		CreateGrids(this, position0, position1, sf::Vector2f(0, 0));

		//Load items displayed
		for (int i = 0; i < NBVAL_Equipment; i++)
		{
			if (playerShip->m_equipment[i] != NULL)
			{
				GameObject* capsule = Enemy::CloneEquipmentIntoGameObject(playerShip->m_equipment[i]);
				m_grids_v2[Trade_EquippedGrid]->InsertObject(capsule, i, false);
			}
		}
		if (playerShip->m_weapon != NULL)
		{
			GameObject* capsule = Enemy::CloneWeaponIntoGameObject(playerShip->m_weapon);
			m_grids_v2[Trade_EquippedGrid]->InsertObject(capsule, NBVAL_Equipment, false);
		}
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
		if (m_playerShip->GetNumberOfBombs() > 0 && m_playerShip->m_hyperspeed_fuel > m_playerShip->m_hyperspeed_fuel_max / BOMB_DEFAULT_NUMBER)
		{
			m_fuelBarOverblock.setSize(sf::Vector2f(1.0f * m_playerShip->m_hyperspeed_fuel_max / BOMB_DEFAULT_NUMBER / m_playerShip->m_hyperspeed_fuel_max * ARMOR_BAR_SIZE_X, FUEL_BAR_SIZE_Y));
			m_fuelBarOverblock.setPosition(sf::Vector2f(m_fuelBar.getPosition().x + m_fuelBar.getSize().x - m_fuelBarOverblock.getSize().x, m_fuelBar.getPosition().y));
		}
		else
		{
			m_fuelBarOverblock.setSize(sf::Vector2f(0, FUEL_BAR_SIZE_Y));
		}
	}

	//Combo
	m_comboBar.setSize(sf::Vector2f((1.0f * m_playerShip->m_combo_count / m_playerShip->m_combo_count_max * COMBO_BAR_SIZE_X), COMBO_BAR_SIZE_Y));
	ostringstream ss_combo;
	ss_combo << "x" << m_playerShip->m_combo_level << " Combo"; 
	m_combo_text.setString(ss_combo.str());

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
	//float quality_graze = m_playerShip->getShipBeastScore() / MAX_BEAST_SCALE * 100;
	float quality_combo = m_playerShip->m_combo_level;
	float quality_hazard = Scene::getSceneBeastScore(m_playerShip->m_currentScene_hazard) / (2 * BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES) * 100;
	ss_beast.precision(0);
	ss_beast << fixed;
	//ss_beast << "Drop quality: +" << quality_graze + quality_hazard << "%";
	ss_beast << "Drop quality: +" << quality_combo + quality_hazard << "% (combo +" << quality_combo << "%, hazard +" << quality_hazard << "%)";
	m_beastscore_text.setString(ss_beast.str());

	//scene name
	if (!m_playerShip->m_currentScene_name.empty())
	{
		ostringstream ss_bg;
		ss_bg << m_playerShip->m_currentScene_name;
		if ((*CurrentGame).m_direction != NO_DIRECTION)
			ss_bg << " (" << m_playerShip->m_currentScene_hazard + 1 << ")";

		m_scene_text.setString(ReplaceAll(ss_bg.str(), "_", " "));
	}

	//framerate
	ostringstream ss_frame;
	ss_frame << "fps= " << (int)(1 / (deltaTime.asMilliseconds() * 0.001));
	m_framerate_text.setString(ss_frame.str());

	//ship global stats
	ostringstream ss_ship_stats;
	float DPS = 0;
	if (m_playerShip->m_weapon != NULL)
		if (m_playerShip->m_weapon->m_shot_mode != NoShotMode)
			DPS += (floor)(1.f / m_playerShip->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_weapon->m_ammunition->m_damage;
		else
			DPS += (floor)(1.f / m_playerShip->m_weapon->m_rate_of_fire * 100) / 100 * m_playerShip->m_weapon->m_ammunition->m_damage * m_playerShip->m_weapon->m_multishot;

	if (m_playerShip->m_bot_list.empty() == false && m_playerShip->m_bot_list.front()->m_weapon != NULL)
	{
		Bot* bot = m_playerShip->m_bot_list.front();
		if (bot->m_weapon->m_ammunition->m_isBeam == false)
			DPS += (floor)(1.f / bot->m_weapon->m_rate_of_fire * 100) / 100 * bot->m_weapon->m_multishot * bot->m_weapon->m_ammunition->m_damage * m_playerShip->m_bot_list.size();
		else
			DPS += (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 * bot->m_weapon->m_multishot * bot->m_weapon->m_ammunition->m_damage * m_playerShip->m_bot_list.size();
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

	//Combo
	screen.draw(m_comboBarContainer);
	screen.draw(m_comboBar);
	screen.draw(m_combo_text);

	if (m_item_stats_panel)
	{
		m_item_stats_panel->Draw(screen);
	}
	if (m_item_stats_panel_compare)
	{
		m_item_stats_panel_compare->Draw(screen);
	}
}