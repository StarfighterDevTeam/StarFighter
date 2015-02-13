#include "Hud.h"

PlayerHud::PlayerHud()
{
	this->no_hazard_value = false;
	this->max_hazard_level_reached = false;
}

void PlayerHud::Init(int m_armor, int m_shield)
{
	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_armor));
	armorBar.setFillColor(sf::Color(0, 250, 50, 128));//green
	armorBar.setOutlineThickness(1);
	armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(0, 0);
	armorBar.setPosition(20, 10);

	shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_shield));
	shieldBar.setFillColor(sf::Color(0, 50, 250, 128));//blue
	shieldBar.setOutlineThickness(1);
	shieldBar.setOutlineColor(sf::Color(255, 255, 255));
	shieldBar.setOrigin(0,0);
	shieldBar.setPosition((40+ARMOR_BAR_SIZE_X),10);

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
		Money.setPosition(50,WINDOW_RESOLUTION_Y-50);

		GrazeScore.setFont(*font);
		GrazeScore.setCharacterSize(14);
		GrazeScore.setColor(_white);
		GrazeScore.setPosition(50, WINDOW_RESOLUTION_Y - 50 - HUD_SCORES_SPACING);

		SceneName.setFont(*font);
		SceneName.setCharacterSize(14);
		SceneName.setColor(_white);
		SceneName.setPosition(50, WINDOW_RESOLUTION_Y - 50 - 2*HUD_SCORES_SPACING);

		//right part
		hazardBreakText = new sf::Text("Hazard\nBreak", *font, 12);
		hazardBreakText->setColor(sf::Color::Red);
		hazardBreakText->setStyle(sf::Text::Bold);
		hazardBreakText->setPosition(WINDOW_RESOLUTION_X-100,HAZARD_BAR_SIZE_Y+20);

		hazardBreakScore = new sf::Text("", *font, 15);
		hazardBreakScore->setColor(sf::Color::Red);
		hazardBreakScore->setStyle(sf::Text::Regular);
		hazardBreakScore->setPosition(SCENE_SIZE_X-100,HAZARD_BAR_SIZE_Y+46);

		framerate = new sf::Text("00", *font2, 15);
		framerate->setColor(sf::Color::Yellow);
		framerate->setStyle(sf::Text::Bold);
		framerate->setPosition(WINDOW_RESOLUTION_X-80,WINDOW_RESOLUTION_Y-30);
	}

	catch( const std::exception & ex ) 
	{
		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Hazard feature (scoring system)
	hazardBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 0));
	hazardBar.setFillColor(sf::Color(250, 0, 50, 128));//red
	hazardBar.setOutlineThickness(1);
	hazardBar.setOutlineColor(sf::Color(255, 255, 255));
	hazardBar.setOrigin(0, 0);
	hazardBar.setPosition(WINDOW_RESOLUTION_X-40, 10);

	//hazardBarMax.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, hazard_break_value));
	hazardBarMax.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
	hazardBarMax.setFillColor(sf::Color::Transparent);//black
	hazardBarMax.setOutlineThickness(1);
	hazardBarMax.setOutlineColor(sf::Color(255, 255, 255, 128));
	hazardBarMax.setOrigin(0, 0);
	hazardBarMax.setPosition(WINDOW_RESOLUTION_X-40, 10);
}

void PlayerHud::Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, int m_hazard_score, int m_hazard_break_value, std::string scene_name, sf::Time deltaTime)
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

	//hazard break bar
	if (m_hazard_break_value != 0)
	{
		no_hazard_value = false;
		hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, (m_hazard_score*HAZARD_BAR_SIZE_Y) / m_hazard_break_value));
	}

	else
	{
		no_hazard_value = true;
	}

	if (m_hazard_level < (HazardLevels::NB_HAZARD_LEVELS - 1))
	{
		max_hazard_level_reached = false;
		ostringstream ss_h1, ss_h2;

		ss_h1 << m_hazard_score;
		ss_h2 << m_hazard_break_value;

		hazardBar.setFillColor(sf::Color(250, 0, 50, 128));//red
		if ((m_hazard_score >= m_hazard_break_value)) // max constraint
		{
			hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
			hazardBar.setFillColor(sf::Color(255, 255, 255, 128));//black
		}

		hazardBreakScore->setString(ss_h1.str() + "/" + ss_h2.str());
	}
	else
	{
		max_hazard_level_reached = true;
	}

	

	//scene name
	ostringstream ss_bg;
	ss_bg << scene_name;
	if (!no_hazard_value)
	{
		ss_bg  << " (" << m_hazard_level+1 << ")";
	} 

	SceneName.setString(ss_bg.str());

	//framerate
	framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));
}

void PlayerHud::Draw(sf::RenderWindow* window)
{
	//Draw all
	window->draw(armorBar);
	window->draw(shieldBar);
	window->draw(Money);
	window->draw(GrazeScore);
	window->draw(SceneName);
	
	if (!no_hazard_value && !max_hazard_level_reached)
	{
		window->draw(hazardBarMax);
		window->draw(hazardBar);
		window->draw(*(hazardBreakText));
		window->draw(*(hazardBreakScore));
	}
	
	window->draw(*(framerate));
}

HudElement::HudElement() : Independant()
{
	this->visible = true;
	this->isOnScene = true;
}

void HudElement::update(sf::Time deltaTime)
{
	Independant::update(deltaTime);

	this->rect.setPosition(this->getPosition());
	this->title.setPosition(this->rect.getPosition().x + this->title_offset.x, this->rect.getPosition().y + this->title_offset.y);
}

void HudElement::Draw(sf::RenderWindow* window)
{
	window->draw(this->rect);
	window->draw(this->title);
}