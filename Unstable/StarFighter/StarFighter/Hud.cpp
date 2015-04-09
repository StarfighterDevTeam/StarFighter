#include "Hud.h"

PlayerHud::PlayerHud()
{
	this->max_hazard_level_reached = false;
	this->focused_item = NULL;

	this->fakeEquipmentGrid = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), true);
	this->fakeShipGrid = ObjectGrid(sf::Vector2f(SHIP_GRID_OFFSET_POS_X, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), true);
	this->equipmentGrid = ObjectGrid(sf::Vector2f(EQUIPMENT_GRID_OFFSET_POS_X, EQUIPMENT_GRID_OFFSET_POS_Y), sf::Vector2i(EQUIPMENT_GRID_NB_LINES, EQUIPMENT_GRID_NB_ROWS), false);
	this->shipGrid = ObjectGrid(sf::Vector2f(SHIP_GRID_OFFSET_POS_X, SHIP_GRID_OFFSET_POS_Y), sf::Vector2i(SHIP_GRID_NB_LINES, SHIP_GRID_NB_ROWS), false);

	this->hud_cursor = new Independant(sf::Vector2f(HUD_LEFT_MARGIN + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)), 
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, HUD_CURSOR_ANIMATION_NUMBER);
}

void PlayerHud::Init(int m_armor, int m_shield)
{
	backgroundColor.setSize(sf::Vector2f(SCENE_SIZE_X * 1.0f / 3, SCENE_SIZE_Y));
	backgroundColor.setFillColor(sf::Color(10, 10, 10, 128));//dark grey
	backgroundColor.setOrigin(0, 0);
	backgroundColor.setPosition(0, 0);

	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_armor));
	armorBar.setFillColor(sf::Color(0, 250, 50, 128));//green
	armorBar.setOutlineThickness(1);
	armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(0, 0);
	armorBar.setPosition(HUD_LEFT_MARGIN, 10);

	if (m_shield > 0)
	{
		shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1 + m_shield));
		shieldBar.setFillColor(sf::Color(0, 50, 250, 128));//blue
		shieldBar.setOutlineThickness(1);
		shieldBar.setOutlineColor(sf::Color(255, 255, 255));
		shieldBar.setOrigin(0, 0);
		shieldBar.setPosition((HUD_LEFT_MARGIN + 20 + ARMOR_BAR_SIZE_X), 10);
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
		//left part
		sf::Color _white = sf::Color::Color(255, 255, 255, 200);//semi-transparent white
		Money.setFont(*font);
		Money.setCharacterSize(20);
		Money.setColor(_white);
		Money.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50);

		GrazeScore.setFont(*font);
		GrazeScore.setCharacterSize(14);
		GrazeScore.setColor(_white);
		GrazeScore.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50 - HUD_SCORES_SPACING);

		SceneName.setFont(*font);
		SceneName.setCharacterSize(14);
		SceneName.setColor(_white);
		SceneName.setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 50 - 2 * HUD_SCORES_SPACING);

		framerate = new sf::Text("00", *font2, 15);
		framerate->setColor(sf::Color::Yellow);
		framerate->setStyle(sf::Text::Bold);
		framerate->setPosition(HUD_LEFT_MARGIN, REF_WINDOW_RESOLUTION_Y - 25);

		//ShipGridTitle.setFont(*font);
		//ShipGridTitle.setCharacterSize(14);
		//ShipGridTitle.setColor(_white);
		//ShipGridTitle.setPosition(HUD_LEFT_MARGIN, SHIP_GRID_OFFSET_POS_Y - 20);
		//
		//EquipmentGridTitle.setFont(*font);
		//EquipmentGridTitle.setCharacterSize(14);
		//EquipmentGridTitle.setColor(_white);
		//EquipmentGridTitle.setPosition(HUD_LEFT_MARGIN, EQUIPMENT_GRID_OFFSET_POS_Y - 20);
	}

	catch( const std::exception & ex ) 
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}
}

void PlayerHud::Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub)
{
	//armor and shield
	if (m_armor <=0)
	{
		armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1));
	}
	else
	{
		armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_armor));
	}

	if (m_shield <=0) 
	{
		shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1));	
	}	
	else
	{
		shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_shield));	
	}

	//money
	ostringstream ss_m;
	ss_m << m_money;
	Money.setString(ss_m.str()+"$");

	//graze
	ostringstream ss_g;
	ss_g << m_graze_count;
	GrazeScore.setString("Graze: "+ss_g.str());

	//scene name
	ostringstream ss_bg;
	ss_bg << scene_name;
	if (!hub)
	{
		ss_bg  << " (" << m_hazard_level+1 << ")";
	} 

	SceneName.setString(ss_bg.str());

	//framerate
	framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));

	//HUD cursor
	hud_cursor->update(deltaTime);

	//HUD panel constraints and movement application
	{
		if (hud_cursor->getPosition().x < hud_cursor->m_size.x / 2)
		{
			hud_cursor->setPosition(hud_cursor->m_size.x / 2, hud_cursor->getPosition().y);
			hud_cursor->speed.x = 0;
		}

		if (hud_cursor->getPosition().x >(SCENE_SIZE_X / 3) - (hud_cursor->m_size.x / 2))
		{
			hud_cursor->setPosition((SCENE_SIZE_X / 3) - (hud_cursor->m_size.x / 2), hud_cursor->getPosition().y);
			hud_cursor->speed.x = 0;
		}

		if (hud_cursor->getPosition().y < hud_cursor->m_size.y / 2)
		{
			hud_cursor->setPosition(hud_cursor->getPosition().x, hud_cursor->m_size.y / 2);
			hud_cursor->speed.y = 0;
		}

		if (hud_cursor->getPosition().y > SCENE_SIZE_Y - (hud_cursor->m_size.y / 2))
		{
			hud_cursor->setPosition(hud_cursor->getPosition().x, SCENE_SIZE_Y - (hud_cursor->m_size.y / 2));
			hud_cursor->speed.y = 0;
		}
	}
	
	//clean old focus
	focused_grid_and_index = sf::Vector2i((int)HudGrid_ShipGrid, -1);
	if (fakeShipGrid.CleanFocus() || fakeEquipmentGrid.CleanFocus())
	{
		//if the focus has been cleaned, that means we must also clean the cursor
		hud_cursor->setAnimationLine(Cursor_NormalState);
	}

	//HUD cursor collides with an item?
	int hovered_index_ = fakeShipGrid.isCursorColling(*hud_cursor);
	if (hovered_index_ < 0)
	{
		//we test the equipment grid
		hovered_index_ = fakeEquipmentGrid.isCursorColling(*hud_cursor);
		if (hovered_index_ > -1)
		{
			//we focus on the hovered grid cell in equipement grid
			fakeEquipmentGrid.HighlightCell(hovered_index_);
			if (equipmentGrid.getCellPointerFromIntIndex(hovered_index_) != NULL)
			{
				hud_cursor->setAnimationLine(Cursor_ActionState);
				focused_item = equipmentGrid.getCellPointerFromIntIndex(hovered_index_);
			}
			else
			{
				hud_cursor->setAnimationLine(Cursor_HighlightState);
				focused_item = NULL;
			}
			focused_grid_and_index = sf::Vector2i((int)HudGrid_EquipmentGrid, hovered_index_);
		}
		else
		{
			//not hovering any of the grids
		}
	}
	else
	{
		//we focus the hovered grid cell in ship grid
		fakeShipGrid.HighlightCell(hovered_index_);
		hud_cursor->setAnimationLine(Cursor_HighlightState);

		if (shipGrid.getCellPointerFromIntIndex(hovered_index_) != NULL)
		{
			focused_item = shipGrid.getCellPointerFromIntIndex(hovered_index_);
		}
		else
		{
			focused_item = NULL;
		}
		focused_grid_and_index = sf::Vector2i((int)HudGrid_ShipGrid, hovered_index_);
	}
		
	//grid titles
	//ShipGridTitle.setString("Ship equipment");
	//EquipmentGridTitle.setString("Stash");
}

void PlayerHud::Draw(sf::RenderTexture& offscreen)
{
	//Draw all
	offscreen.draw(backgroundColor);

	offscreen.draw(armorBar);
	if (this->has_shield)
	{
		offscreen.draw(shieldBar);
	}
	offscreen.draw(Money);
	offscreen.draw(GrazeScore);
	offscreen.draw(SceneName);
	
	offscreen.draw(*(framerate));
	//offscreen.draw(ShipGridTitle);
	//offscreen.draw(EquipmentGridTitle);

	fakeEquipmentGrid.Draw(offscreen);
	fakeShipGrid.Draw(offscreen);
	equipmentGrid.Draw(offscreen);
	shipGrid.Draw(offscreen);

	if (hud_cursor->visible)
	{
		offscreen.draw(*hud_cursor);
	}
}