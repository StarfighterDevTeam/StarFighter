#include "Hud.h"

PlayerHud::PlayerHud()
{
	this->max_hazard_level_reached = false;
	this->focused_item = NULL;
	this->has_focus = false;
	this->has_prioritary_cursor_feedback = false;

	this->fakeEquipmentGrid = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), true);
	this->fakeShipGrid = ObjectGrid(sf::Vector2f(SHIP_GRID_OFFSET_POS_X, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), true);
	this->equipmentGrid = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false);
	this->shipGrid = ObjectGrid(sf::Vector2f(SHIP_GRID_OFFSET_POS_X, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false);

	this->hud_cursor = new GameObject(sf::Vector2f(HUD_LEFT_MARGIN + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)), 
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8+1));
}

void PlayerHud::Init(int m_armor, int m_shield, int xp, int xp_max)
{
	backgroundColor.setSize(sf::Vector2f(SCENE_SIZE_X * 1.0f / 3, SCENE_SIZE_Y));
	backgroundColor.setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	backgroundColor.setOrigin(0, 0);
	backgroundColor.setPosition(0, 0);

	armorBar.setSize(sf::Vector2f(1 + m_armor, ARMOR_BAR_SIZE_Y));
	armorBar.setFillColor(sf::Color(COLOR_GREEN_R_VALUE, COLOR_GREEN_G_VALUE, COLOR_GREEN_B_VALUE, COLOR_GREEN_A_VALUE));//green
	//armorBar.setOutlineThickness(1);
	//armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(0, 0);
	armorBar.setPosition(HUD_LEFT_MARGIN, 10);

	shieldBar.setSize(sf::Vector2f(1 + m_shield, SHIELD_BAR_SIZE_Y));
	shieldBar.setFillColor(sf::Color(COLOR_BLUE_R_VALUE, COLOR_BLUE_G_VALUE, COLOR_BLUE_B_VALUE, COLOR_BLUE_A_VALUE));//blue
	//shieldBar.setOutlineThickness(1);
	//shieldBar.setOutlineColor(sf::Color(255, 255, 255));
	shieldBar.setOrigin(0, 0);
	shieldBar.setPosition(HUD_LEFT_MARGIN, 20 + ARMOR_BAR_SIZE_Y);

	armorBarContainer.setSize(sf::Vector2f(1 + m_armor, ARMOR_BAR_SIZE_Y));
	armorBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
	armorBarContainer.setOutlineThickness(1);
	armorBarContainer.setOutlineColor(sf::Color(255, 255, 255));
	armorBarContainer.setOrigin(0, 0);
	armorBarContainer.setPosition(HUD_LEFT_MARGIN, 10);

	shieldBarContainer.setSize(sf::Vector2f(1 + m_shield, SHIELD_BAR_SIZE_Y));
	shieldBarContainer.setFillColor(sf::Color(0, 0, 0, 0));
	shieldBarContainer.setOutlineThickness(1);
	shieldBarContainer.setOutlineColor(sf::Color(255, 255, 255));
	shieldBarContainer.setOrigin(0, 0);
	shieldBarContainer.setPosition(HUD_LEFT_MARGIN, 20 + ARMOR_BAR_SIZE_Y);

	xpBar.setSize(sf::Vector2f((1.0f * xp / xp_max) * XP_BAR_SIZE_X, XP_BAR_SIZE_Y));
	xpBar.setFillColor(sf::Color(COLOR_LIGHT_BLUE_R_VALUE, COLOR_LIGHT_BLUE_G_VALUE, COLOR_LIGHT_BLUE_B_VALUE, COLOR_LIGHT_BLUE_A_VALUE));//light blue
	xpBar.setOrigin(0, 0);
	xpBar.setPosition(HUD_LEFT_MARGIN, 20 + (2 * ARMOR_BAR_SIZE_Y) + 20);

	itemStatsPanel.setSize(sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y));
	itemStatsPanel.setFillColor(sf::Color(70, 70, 70, 128));//like grey
	itemStatsPanel.setOrigin(0, 0);
	itemStatsPanel.setPosition(HUD_LEFT_MARGIN, (3 * ARMOR_BAR_SIZE_Y) + 40);

	if (m_shield > 0)
	{
		this->has_shield = true;
	}
	else
	{
		this->has_shield = false;
	}

	try 
	{
		font = new sf::Font();
		if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{
			// error
			//TODO: font loader
		}

		font2 = new sf::Font();
		if (!font2->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}

		sf::Color _white = sf::Color::Color(255, 255, 255, 200);//semi-transparent white

		itemStatsText.setFont(*font2);
		itemStatsText.setCharacterSize(18);
		itemStatsText.setColor(_white);
		itemStatsText.setPosition(HUD_LEFT_MARGIN + 5, (3 * ARMOR_BAR_SIZE_Y) + 40 + 5);
		
		LifeText.setFont(*font);
		LifeText.setCharacterSize(10);
		LifeText.setColor(_white);
		LifeText.setPosition(HUD_LEFT_MARGIN + ARMOR_BAR_SIZE_X / 2, 10);

		ShieldText.setFont(*font);
		ShieldText.setCharacterSize(10);
		ShieldText.setColor(_white);
		ShieldText.setPosition(HUD_LEFT_MARGIN + ARMOR_BAR_SIZE_X / 2, 20 + ARMOR_BAR_SIZE_Y);

		Money.setFont(*font);
		Money.setCharacterSize(20);
		Money.setColor(_white);
		Money.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50);

		GrazeScore.setFont(*font);
		GrazeScore.setCharacterSize(14);
		GrazeScore.setColor(_white);
		GrazeScore.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50 - HUD_SCORES_SPACING);

		Level.setFont(*font);
		Level.setCharacterSize(14);
		Level.setColor(_white);
		Level.setPosition(HUD_LEFT_MARGIN, 20 + (2 * ARMOR_BAR_SIZE_Y) + 20);

		SceneName.setFont(*font);
		SceneName.setCharacterSize(14);
		SceneName.setColor(_white);
		SceneName.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50 - 2 * HUD_SCORES_SPACING);

		framerate = new sf::Text("00", *font2, 15);
		framerate->setColor(sf::Color::Yellow);
		framerate->setStyle(sf::Text::Bold);
		framerate->setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 25);
	}

	catch( const std::exception & ex ) 
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}
}

void PlayerHud::GarbageObjectInGrid(int grid_id, int index)
{
	switch (grid_id)
	{
		case HudGrid_ShipGrid:
		{
			delete shipGrid.getCellPointerFromIntIndex(index);
			shipGrid.setCellPointerForIntIndex(index, NULL);
			break;
		}
		case HudGrid_EquipmentGrid:
		{
			delete equipmentGrid.getCellPointerFromIntIndex(index);
			equipmentGrid.setCellPointerForIntIndex(index, NULL);
			break;
		}
	}

	this->focused_item = NULL;
}

void PlayerHud::setRemovingCursorAnimation(CursorFeedbackStates animation_index)
{
	this->hud_cursor->setAnimationLine(animation_index);
}

void PlayerHud::Update(int m_armor, int m_armor_max, int m_shield, int m_shield_max, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, int level, int level_max, int xp, int xp_max, sf::Time deltaTime, bool hub,
	int focused_item_type, string f_name, int f_level, int f_xp, float f_max_speed, float f_hyperspeed, int f_armor, int f_shield, int f_shield_regen,
	int f_damage, bool f_bot, float f_ammo_speed, PatternType f_pattern,
	int f_multishot, int f_xspread, float f_rate_of_fire, ShotMode f_shot_mode, float f_dispersion, int f_rafale, float f_rafale_cooldown, TargetSeaking f_target_seaking)
{

	//armor and shield
	if (m_armor <=0)
	{
		armorBar.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
		armorBarContainer.setSize(sf::Vector2f(1, ARMOR_BAR_SIZE_Y));
	}
	else
	{
		if (m_armor < m_shield)
		{
			armorBar.setSize(sf::Vector2f(1 + (1.0f * m_armor / m_armor_max * ARMOR_BAR_SIZE_X * m_armor_max / m_shield_max), ARMOR_BAR_SIZE_Y));
			armorBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X * m_armor_max / m_shield_max, ARMOR_BAR_SIZE_Y));
		}
		else
		{
			armorBar.setSize(sf::Vector2f(1 + (1.0f * m_armor / m_armor_max * ARMOR_BAR_SIZE_X), ARMOR_BAR_SIZE_Y));
			armorBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, ARMOR_BAR_SIZE_Y));
		}
	}

	if (m_shield <=0) 
	{
		shieldBar.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
		shieldBarContainer.setSize(sf::Vector2f(1, SHIELD_BAR_SIZE_Y));
		this->has_shield = false;
	}	
	else
	{
		if (m_shield < m_armor)
		{
			shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_shield / m_shield_max * ARMOR_BAR_SIZE_X * m_shield_max / m_armor_max), SHIELD_BAR_SIZE_Y));
			shieldBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X * m_shield_max / m_armor_max, SHIELD_BAR_SIZE_Y));
		}
		else
		{
			shieldBar.setSize(sf::Vector2f(1 + (1.0f * m_shield / m_shield_max * ARMOR_BAR_SIZE_X), SHIELD_BAR_SIZE_Y));
			shieldBarContainer.setSize(sf::Vector2f(1 + ARMOR_BAR_SIZE_X, SHIELD_BAR_SIZE_Y));
		}
		this->has_shield = true;
	}

	ostringstream ss_life;
	ss_life << m_armor;// << " / " << m_armor_max;
	LifeText.setString(ss_life.str());
	LifeText.setPosition(HUD_LEFT_MARGIN + armorBar.getGlobalBounds().width / 2 - LifeText.getGlobalBounds().width / 2, 10 + LifeText.getGlobalBounds().height / 2);

	ostringstream ss_shield;
	ss_shield << m_shield;// << " / " << m_shield_max;
	ShieldText.setString(ss_shield.str());
	ShieldText.setPosition(HUD_LEFT_MARGIN + shieldBar.getGlobalBounds().width / 2 - ShieldText.getGlobalBounds().width / 2, 20 + ARMOR_BAR_SIZE_Y + ShieldText.getGlobalBounds().height / 2);

	//money
	ostringstream ss_m;
	ss_m << m_money;
	Money.setString(ss_m.str()+"$");

	//graze
	ostringstream ss_g;
	ss_g << m_graze_count;
	GrazeScore.setString("Graze: "+ ss_g.str());

	//level
	ostringstream ss_slash;
	ss_slash << "Level " << level;
	if (level_max > -1)
	{
		ss_slash  << " / " << level_max;
	}
	ss_slash << " (XP: " << xp << " / " << xp_max << ")";

	Level.setString(ss_slash.str());

	xpBar.setSize(sf::Vector2f((1.0f * xp / xp_max) * XP_BAR_SIZE_X, SHIELD_BAR_SIZE_Y));

	//scene name
	if (!hub)
	{
		ostringstream ss_bg;
		ss_bg  << " (" << m_hazard_level+1 << ")";
		SceneName.setString(scene_name + ss_bg.str());
	} 
	else
	{
		SceneName.setString(scene_name);
	}

	//framerate
	framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));

	//HUD cursor
	hud_cursor->update(deltaTime);

	//HUD panel constraints and movement application
	{
		if (hud_cursor->getPosition().x < hud_cursor->m_size.x / 2)
		{
			hud_cursor->setPosition(hud_cursor->m_size.x / 2, hud_cursor->getPosition().y);
			hud_cursor->m_speed.x = 0;
		}

		if (hud_cursor->getPosition().x >(SCENE_SIZE_X / 3) - (hud_cursor->m_size.x / 2))
		{
			hud_cursor->setPosition((SCENE_SIZE_X / 3) - (hud_cursor->m_size.x / 2), hud_cursor->getPosition().y);
			hud_cursor->m_speed.x = 0;
		}

		if (hud_cursor->getPosition().y < hud_cursor->m_size.y / 2)
		{
			hud_cursor->setPosition(hud_cursor->getPosition().x, hud_cursor->m_size.y / 2);
			hud_cursor->m_speed.y = 0;
		}

		if (hud_cursor->getPosition().y > SCENE_SIZE_Y - (hud_cursor->m_size.y / 2))
		{
			hud_cursor->setPosition(hud_cursor->getPosition().x, SCENE_SIZE_Y - (hud_cursor->m_size.y / 2));
			hud_cursor->m_speed.y = 0;
		}
	}
	
	//clean old focus
	hud_cursor->m_visible = has_focus;

	focused_grid_and_index = sf::Vector2i((int)HudGrid_NoFocus, -1);
	if (fakeShipGrid.CleanFocus() || fakeEquipmentGrid.CleanFocus())
	{
		//if the focus has been cleaned, that means we must also clean the cursor
		if (!has_prioritary_cursor_feedback)
		{
			hud_cursor->setAnimationLine(Cursor_NormalState);
		}
		
	}

	if (has_focus)
	{
		//HUD cursor collides with an item?
		int hovered_index_ = fakeShipGrid.isCursorColling(*hud_cursor);
		if (hovered_index_ < 0)
		{
			//we test the equipment grid
			hovered_index_ = fakeEquipmentGrid.isCursorColling(*hud_cursor);
			if (hovered_index_ > -1)//the equipment grid is focused
			{
				fakeEquipmentGrid.HighlightCell(hovered_index_);
				if (equipmentGrid.getCellPointerFromIntIndex(hovered_index_) != NULL)
				{
					if (!has_prioritary_cursor_feedback)
					{
						hud_cursor->setAnimationLine(Cursor_ActionState);
					}
					if (focused_item != equipmentGrid.getCellPointerFromIntIndex(hovered_index_))
					{
						focused_item = equipmentGrid.getCellPointerFromIntIndex(hovered_index_);
					}
				}
				else
				{
					if (!has_prioritary_cursor_feedback)
					{
						hud_cursor->setAnimationLine(Cursor_HighlightState);
					}
					focused_item = NULL;
				}
				focused_grid_and_index = sf::Vector2i((int)HudGrid_EquipmentGrid, hovered_index_);
			}
			else//no focus at all: the cursor is on an empty space
			{
				focused_grid_and_index = sf::Vector2i((int)HudGrid_NoFocus, hovered_index_);
				focused_item = NULL;
				if (!has_prioritary_cursor_feedback)
				{
					hud_cursor->setAnimationLine(Cursor_NormalState);
				}
			}
		}
		else//the ship grid is focused
		{
			
			fakeShipGrid.HighlightCell(hovered_index_);
			if (!has_prioritary_cursor_feedback)
			{
				hud_cursor->setAnimationLine(Cursor_HighlightState);
			}

			if (shipGrid.getCellPointerFromIntIndex(hovered_index_) != NULL)
			{
				if (focused_item != shipGrid.getCellPointerFromIntIndex(hovered_index_))
				{
					focused_item = shipGrid.getCellPointerFromIntIndex(hovered_index_);
				}
			}
			else
			{
				focused_item = NULL;
			}
			focused_grid_and_index = sf::Vector2i((int)HudGrid_ShipGrid, hovered_index_);
		}
	}
	else
	{
		focused_item = NULL;
	}
	
	//ITEM STATS PANEL DISPLAY
	ostringstream ss_stats;
	if (this->focused_item != NULL)
	{
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
	}

	itemStatsText.setString(ss_stats.str());
	//todo
}

void PlayerHud::Draw(sf::RenderTexture& offscreen)
{
	//Draw all
	offscreen.draw(backgroundColor);

	offscreen.draw(armorBar);
	offscreen.draw(armorBarContainer);
	offscreen.draw(LifeText);
	if (this->has_shield)
	{
		offscreen.draw(shieldBar);
		offscreen.draw(shieldBarContainer);
		offscreen.draw(ShieldText);
	}
	offscreen.draw(Money);
	offscreen.draw(GrazeScore);
	offscreen.draw(xpBar);
	offscreen.draw(Level);
	offscreen.draw(SceneName);

	if (this->focused_item != NULL && this->has_focus)
	{
		offscreen.draw(itemStatsPanel);
		offscreen.draw(itemStatsText);
	}
	
	offscreen.draw(*(framerate));

	fakeEquipmentGrid.Draw(offscreen);
	fakeShipGrid.Draw(offscreen);
	equipmentGrid.Draw(offscreen);
	shipGrid.Draw(offscreen);

	if (this->has_focus)
	{
		offscreen.draw(*hud_cursor);
	}
}