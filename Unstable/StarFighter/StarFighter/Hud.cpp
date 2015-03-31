#include "Hud.h"

ObjectData::ObjectData()
{

}

ObjectData::ObjectData(std::string textureName)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture(textureName, EQUIPMENT_GRID_SLOT_SIZE, EQUIPMENT_GRID_SLOT_SIZE);
	this->textureName = textureName;

	this->setOrigin(EQUIPMENT_GRID_SLOT_SIZE / 2, EQUIPMENT_GRID_SLOT_SIZE / 2);
}

ObjectGrid::ObjectGrid()
{
	for (size_t i = 0; i < EQUIPMENT_GRID_NB_LINES; i++)
	{
		for (size_t j = 0; j < EQUIPMENT_GRID_NB_ROWS; j++)
		{
			grid[i][j] = NULL;
		}
	}
}

void ObjectGrid::fakeGridFill()
{
	for (size_t i = 0; i < EQUIPMENT_GRID_NB_LINES; i++)
	{
		for (size_t j = 0; j < EQUIPMENT_GRID_NB_ROWS; j++)
		{
			Independant* empty_slot = new Independant(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(EQUIPMENT_GRID_SLOT_SIZE, EQUIPMENT_GRID_SLOT_SIZE));
			empty_slot->setPosition(sf::Vector2f((EQUIPMENT_GRID_SLOT_SIZE / 2) + EQUIPMENT_GRID_OFFSET_POS_X + (j * EQUIPMENT_GRID_SLOT_SIZE), (EQUIPMENT_GRID_SLOT_SIZE / 2) + EQUIPMENT_GRID_OFFSET_POS_Y + (i * EQUIPMENT_GRID_SLOT_SIZE)));
			grid[i][j] = empty_slot;
		}
	}
}

bool ObjectGrid::insertObject(Independant& object)
{
	for (size_t i = 0; i < EQUIPMENT_GRID_NB_LINES; i++)
	{
		for (size_t j = 0; j < EQUIPMENT_GRID_NB_ROWS; j++)
		{
			if (grid[i][j] == NULL)
			{
				grid[i][j] = &object;
				grid[i][j]->setPosition(sf::Vector2f((EQUIPMENT_GRID_SLOT_SIZE / 2) + EQUIPMENT_GRID_OFFSET_POS_X + (j * EQUIPMENT_GRID_SLOT_SIZE), (EQUIPMENT_GRID_SLOT_SIZE / 2) + EQUIPMENT_GRID_OFFSET_POS_Y + (i * EQUIPMENT_GRID_SLOT_SIZE)));

				return true;
			}
		}
	}
	//no free slot found
	return false;
}

void ObjectGrid::Draw(sf::RenderTexture& offscreen)
{
	for (size_t i = 0; i < EQUIPMENT_GRID_NB_LINES; i++)
	{
		for (size_t j = 0; j < EQUIPMENT_GRID_NB_ROWS; j++)
		{
			if (grid[i][j] != NULL)
			{
				offscreen.draw(*grid[i][j]);
			}
		}
	}
}

PlayerHud::PlayerHud()
{
	this->max_hazard_level_reached = false;
	this->fakeGrid.fakeGridFill();
	
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

	fakeGrid.Draw(offscreen);
	equipmentGrid.Draw(offscreen);
}