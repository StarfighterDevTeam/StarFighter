#include "SFHUDPanel.h"

extern Game* CurrentGame;

//HUD PANEL
SFHUDPanel::SFHUDPanel(sf::Vector2f size, Ship* playership) : SFInventoryPanel(size, playership, SFPanel_HUD)
{
	setOrigin(0, 0);
	setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	setOutlineThickness(0);
	setPosition(SCENE_SIZE_X, 0);

	//cached full-panel background, drawn every frame in Draw() without being rebuilt each time
	m_black_background.setSize(this->getSize());
	m_black_background.setOrigin(this->getOrigin());
	m_black_background.setPosition(this->getPosition());
	m_black_background.setFillColor(sf::Color(0, 0, 0, 255));

	//sentinel values (armor/shield/money/crystals/level are never negative in-game) so the first
	//Update() call always rebuilds the displayed texts
	m_last_armor = -1;
	m_last_armor_max = -1;
	m_last_shield = -1;
	m_last_shield_max = -1;
	m_last_money = -1;
	m_last_crystals = -1;
	m_last_scene_name = "";
	m_last_level = -1;

	if (playership)
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

		m_xpBar.setSize(sf::Vector2f((1.0f * playership->m_xp / playership->m_xp_max) * XP_BAR_SIZE_X, XP_BAR_SIZE_Y));
		m_xpBar.setFillColor(sf::Color(COLOR_LIGHT_BLUE_R_VALUE, COLOR_LIGHT_BLUE_G_VALUE, COLOR_LIGHT_BLUE_B_VALUE, COLOR_LIGHT_BLUE_A_VALUE));//light blue
		m_xpBar.setOrigin(0, 0);

		m_comboBar.setSize(sf::Vector2f((1.0f * playership->m_combo_count / playership->m_combo_count_max) * COMBO_BAR_SIZE_X, COMBO_BAR_SIZE_Y));
		m_comboBar.setFillColor(sf::Color(COLOR_LIGHT_BLUE_R_VALUE, COLOR_LIGHT_BLUE_G_VALUE, COLOR_LIGHT_BLUE_B_VALUE, COLOR_LIGHT_BLUE_A_VALUE));//light blue
		m_comboBar.setOrigin(0, 0);

		m_comboBarContainer.setSize(sf::Vector2f(COMBO_BAR_SIZE_X, COMBO_BAR_SIZE_Y));
		m_comboBarContainer.setFillColor(sf::Color(0, 0, 0, 255));//light blue
		m_comboBarContainer.setOutlineThickness(1);
		m_comboBarContainer.setOutlineColor(sf::Color(255, 255, 255));
		m_comboBarContainer.setOrigin(0, 0);

		sf::Color _white = sf::Color::Color(255, 255, 255, 180);//semi-transparent white

		m_text.setColor(_white);

		m_life_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_life_text.setCharacterSize(10);
		m_life_text.setColor(_white);

		m_shield_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_shield_text.setCharacterSize(10);
		m_shield_text.setColor(_white);

		//lower part
		m_money_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_money_text.setCharacterSize(20);
		m_money_text.setColor(_white);

		m_scene_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_scene_text.setCharacterSize(14);
		m_scene_text.setColor(_white);

		m_framerate_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		m_framerate_text.setCharacterSize(15);
		m_framerate_text.setColor(sf::Color::Yellow);

		//m_text's font is already set by the SFInventoryPanel base constructor

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

		text_height += INTERACTION_INTERBLOCK + ARMOR_BAR_SIZE_Y;
		m_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += 2 * INTERACTION_INTERBLOCK;// STASH_GRID_NB_LINES * GRID_SLOT_SIZE + INTERACTION_INTERBLOCK;
		m_money_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += m_money_text.getCharacterSize();
		m_level_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_SHOP_INTERLINE + m_level_text.getCharacterSize();
		m_scene_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);

		text_height += INTERACTION_INTERBLOCK + m_beastscore_text.getCharacterSize();
		m_framerate_text.setPosition(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES, text_height);
		text_height += INTERACTION_INTERBLOCK + m_framerate_text.getCharacterSize();

		//crystals
		m_crystal = GameObject(getPosition(), sf::Vector2f(0, 0), "2D/Equipment/crystal.png", sf::Vector2f(48, 48));
		m_crystal.setPosition(sf::Vector2f(SCENE_SIZE_X + INTERACTION_PANEL_MARGIN_SIDES + m_crystal.m_size.x * 0.5, 370));

		m_crystals_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_crystals_text.setCharacterSize(15);
		m_crystals_text.setColor(_white);
		m_crystals_text.setPosition(sf::Vector2f(m_crystal.getPosition().x + m_crystal.m_size.x * 0.5 + 8, m_crystal.getPosition().y));

		//Scene progression bar
		m_progressionBar.setSize(sf::Vector2f(SCENE_PROGRESSION_BAR_WIDTH, SCENE_PROGRESSION_BAR_HEIGHT));
		m_progressionBar.setOrigin(sf::Vector2f(m_progressionBar.getSize().x * 0.5, m_progressionBar.getSize().y * 0.5));
		m_progressionBar.setFillColor(sf::Color(255, 255, 255, 200));
		m_progressionBar.setPosition(sf::Vector2f(SCENE_SIZE_X + INTERACTION_PANEL_MARGIN_SIDES + ASSUMED_SHIP_SIZE + SCENE_PROGRESSION_BAR_WIDTH * 0.5 + SCENE_PROGRESSION_BAR_OFFSET_X, SCENE_PROGRESSION_BAR_POS_Y));
	
		m_progressionExit.setSize(sf::Vector2f(24, 24));
		m_progressionExit.setOrigin(sf::Vector2f(m_progressionExit.getSize().x * 0.5, m_progressionExit.getSize().y * 0.5));
		m_progressionExit.setFillColor(sf::Color::Red);
		m_progressionExit.setPosition(sf::Vector2f(m_progressionBar.getPosition().x, m_progressionBar.getPosition().y - m_progressionBar.getSize().y * 0.5));

		sf::Vector2f ship_size = playership->m_fake_ship ? playership->m_fake_ship->m_size : playership->m_size;
		string ship_texture = playership->m_fake_ship ? playership->m_fake_ship->m_textureName : playership->m_textureName;
		int frameNumber = playership->m_fake_ship ? playership->m_fake_ship->m_frameNumber : playership->m_frameNumber;
		int animationNumber = playership->m_fake_ship ? playership->m_fake_ship->m_animationNumber : playership->m_animationNumber;

		m_ship = GameObject(getPosition(), sf::Vector2f(0, 0), ship_texture, ship_size, sf::Vector2f(ship_size.x / 2, ship_size.y / 2), animationNumber, frameNumber);
		m_ship.setPosition(sf::Vector2f(m_progressionBar.getPosition().x, m_progressionBar.getPosition().y + m_progressionBar.getSize().y * 0.5));
		m_ship.setColor(sf::Color(255, 255, 255, 255));
		m_ship.setScale(STELLARMAP_SHIP_MINIATURE_SCALE, STELLARMAP_SHIP_MINIATURE_SCALE);

		m_progression_text.setFont(*(*CurrentGame).m_font[Font_Terminator]);
		m_progression_text.setCharacterSize(14);
		m_progression_text.setColor(_white);
	}
}

SFHUDPanel::~SFHUDPanel()
{

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

	if (!m_playership)
	{
		return;
	}

	//armor
	if (m_playership->m_armor_max <= 0)
	{
		m_armorBar.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
		m_armorBarContainer.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
	}
	else
	{
		if (m_playership->m_armor_max < m_playership->m_shield_max)
		{
			m_armorBar.setSize(sf::Vector2f(1 + (1.0f * m_playership->m_armor / m_playership->m_armor_max * ARMOR_BAR_SIZE_X * m_playership->m_armor_max / m_playership->m_shield_max), ARMOR_BAR_SIZE_Y));
			m_armorBarContainer.setSize(sf::Vector2f(1 + 1.0f * ARMOR_BAR_SIZE_X * m_playership->m_armor_max / m_playership->m_shield_max, ARMOR_BAR_SIZE_Y));
		}
		else
		{
			m_armorBar.setSize(sf::Vector2f(1 + (1.0f * m_playership->m_armor / m_playership->m_armor_max * ARMOR_BAR_SIZE_X), ARMOR_BAR_SIZE_Y));
			m_armorBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, ARMOR_BAR_SIZE_Y));
		}
	}

	//shield
	if (m_playership->m_shield_max <= 0)
	{
		m_shieldBar.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
		m_shieldBarContainer.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
	}
	else
	{
		if (m_playership->m_shield_max < m_playership->m_armor_max)
		{
			m_shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_playership->m_shield / m_playership->m_shield_max * ARMOR_BAR_SIZE_X * m_playership->m_shield_max / m_playership->m_armor_max), SHIELD_BAR_SIZE_Y));
			m_shieldBarContainer.setSize(sf::Vector2f(1 + 1.0f * ARMOR_BAR_SIZE_X * m_playership->m_shield_max / m_playership->m_armor_max, SHIELD_BAR_SIZE_Y));
		}
		else
		{
			m_shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_playership->m_shield / m_playership->m_shield_max * ARMOR_BAR_SIZE_X), SHIELD_BAR_SIZE_Y));
			m_shieldBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, SHIELD_BAR_SIZE_Y));
		}
	}

	//life (only rebuild the text when the displayed value actually changes)
	if (m_playership->m_armor != m_last_armor || m_playership->m_armor_max != m_last_armor_max)
	{
		m_last_armor = m_playership->m_armor;
		m_last_armor_max = m_playership->m_armor_max;

		ostringstream ss_life;
		ss_life << m_last_armor << "/" << m_last_armor_max;
		m_life_text.setString(ss_life.str());
	}
	m_life_text.setPosition(m_armorBarContainer.getPosition().x + m_armorBarContainer.getSize().x / 2 - m_life_text.getGlobalBounds().width / 2, m_armorBarContainer.getPosition().y + m_life_text.getGlobalBounds().height / 2);

	//shield (only rebuild the text when the displayed value actually changes)
	if (m_playership->m_shield != m_last_shield || m_playership->m_shield_max != m_last_shield_max)
	{
		m_last_shield = m_playership->m_shield;
		m_last_shield_max = m_playership->m_shield_max;

		ostringstream ss_shield;
		ss_shield << m_last_shield << "/" << m_last_shield_max;
		m_shield_text.setString(ss_shield.str());
	}
	m_shield_text.setPosition(m_shieldBarContainer.getPosition().x + m_shieldBarContainer.getSize().x / 2 - m_shield_text.getGlobalBounds().width / 2, m_shieldBarContainer.getPosition().y + m_shield_text.getGlobalBounds().height / 2);

	//money
	if (m_playership->m_money != m_last_money)
	{
		m_last_money = m_playership->m_money;
		m_money_text.setString("$ " + to_string(m_last_money));
	}

	//crystals
	if (m_playership->m_crystals != m_last_crystals)
	{
		m_last_crystals = m_playership->m_crystals;
		m_crystals_text.setString(to_string(m_last_crystals));
	}

	//scene name
	if (!m_playership->m_currentScene_name.empty() && (m_playership->m_currentScene_name != m_last_scene_name || m_playership->m_level != m_last_level))
	{
		m_last_scene_name = m_playership->m_currentScene_name;
		m_last_level = m_playership->m_level;

		m_scene_text.setString(ReplaceAll(m_last_scene_name, "_", " ") + " (lvl " + to_string(m_last_level) + ")");
	}

	//framerate
	ostringstream ss_frame;
	float deltaSeconds = deltaTime.asSeconds();
	ss_frame << "fps= " << (deltaSeconds > 0 ? (int)(1.f / deltaSeconds) : 0);
	m_framerate_text.setString(ss_frame.str());

	//ship global stats
	ostringstream ss_ship_stats;
	float DPS = 0;
	if (m_playership->m_weapon != NULL)

		if (m_playership->m_weapon->m_ammunition->m_isBeam == false)
			DPS += (floor)(1.f / m_playership->m_weapon->m_rate_of_fire * 100) / 100 * m_playership->m_weapon->m_multishot * m_playership->m_weapon->m_ammunition->m_damage;
		else
			DPS += (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 * m_playership->m_weapon->m_multishot * m_playership->m_weapon->m_ammunition->m_damage;

	if (m_playership->m_bot_list.empty() == false && m_playership->m_bot_list.front()->m_weapon != NULL)
	{
		Bot* bot = m_playership->m_bot_list.front();
		if (bot->m_weapon->m_ammunition->m_isBeam == false)
			DPS += (floor)(1.f / bot->m_weapon->m_rate_of_fire * 100) / 100 * bot->m_weapon->m_multishot * bot->m_weapon->m_ammunition->m_damage * m_playership->m_bot_list.size();
		else
			DPS += (floor)(1.f / TIME_BETWEEN_BEAM_DAMAGE_TICK * 100) / 100 * bot->m_weapon->m_multishot * bot->m_weapon->m_ammunition->m_damage * m_playership->m_bot_list.size();
	}

	ss_ship_stats << "DPS: " << DPS;
	//ss_ship_stats << "\nContact damage: " << m_playership->m_damage << "\nHyperspeed: " << m_playership->m_hyperspeed << "\nFuel: " << m_playership->m_hyperspeed_fuel_max
	//	<< "\nShield regen: " << m_playership->m_shield_regen << "/sec" << "\nShield recovery: " << m_playership->m_shield_recovery_time << "sec";
	m_text.setString(ss_ship_stats.str());

	//Scene progression bar
	if (m_playership->m_armor > 0)
	{
		float progression_ratio = ProrataBetweenThreshold((*CurrentGame).m_background->getPosition().y, sf::Vector2f(-(*CurrentGame).m_background->m_size.y * 0.5 + SCENE_SIZE_Y, (*CurrentGame).m_background->m_size.y * 0.5));
		m_ship.setPosition(sf::Vector2f(m_progressionBar.getPosition().x, m_progressionBar.getPosition().y + m_progressionBar.getSize().y * (0.5 - progression_ratio)));
		m_progression_text.setString(to_string((int)(progression_ratio * 100)) + " %");
		m_progression_text.setPosition(sf::Vector2f(m_ship.getPosition().x - m_ship.m_size.x * 0.5 - m_progression_text.getGlobalBounds().width - 1, m_ship.getPosition().y));
	}
	
	for (int i = 0; i < 4; i++)
	{
		string next_linked_scene = (*CurrentGame).m_generalScenesConfig[m_playership->m_currentScene_name][SCENE_LINK_UP + i];
		if (next_linked_scene.compare("0") != 0)
		{
			if ((bool)stoi((*CurrentGame).m_generalScenesConfig[next_linked_scene][SCENE_IS_HUB]) == true)
				m_progressionExit.setFillColor(sf::Color::Red);
			else
				m_progressionExit.setFillColor(sf::Color(255, 255, 255, 0));

			break;
		}
	}
}

void SFHUDPanel::UpdateUpgradeIcons()
{
	m_upgrades_icons.clear();

	int i = 0;
	for (vector<string>::iterator it = m_playership->m_upgrades_short.begin(); it != m_playership->m_upgrades_short.end(); it++)
	{
		sf::Vector2f pos = sf::Vector2f(getPosition().x + INTERACTION_PANEL_MARGIN_SIDES + UPGRADES_PICTURE_WIDTH * 0.5 + i * (UPGRADES_PICTURE_WIDTH + UPGRADES_PICTURES_INTERLINE), 300);
		GameObject icon(pos, sf::Vector2f(0, 0), (*CurrentGame).m_upgradesConfig[*it][UPGRADE_IMAGE], sf::Vector2f(UPGRADES_PICTURE_WIDTH, UPGRADES_PICTURE_HEIGHT));
		m_upgrades_icons.push_back(icon);
		i++;
	}
}

void SFHUDPanel::Draw(sf::RenderTexture& screen)
{
	screen.draw(m_black_background);

	screen.draw(m_armorBarContainer);
	screen.draw(m_armorBar);
	screen.draw(m_life_text);
	if (m_playership && m_playership->m_shield_max > 0)
	{
		screen.draw(m_shieldBarContainer);
		screen.draw(m_shieldBar);
		screen.draw(m_shield_text);
	}

	screen.draw(m_money_text);
	screen.draw(m_crystal);
	screen.draw(m_crystals_text);
	screen.draw(m_scene_text);
	screen.draw(m_framerate_text);
	screen.draw(m_text);

	for (GameObject icon : m_upgrades_icons)
		icon.Draw(screen);

	if ((*CurrentGame).m_gameloop_state == SCROLLING)
	{
		screen.draw(m_progressionBar);
		screen.draw(m_progressionExit);
		screen.draw(m_progression_text);
		m_ship.Draw(screen);
	}
}