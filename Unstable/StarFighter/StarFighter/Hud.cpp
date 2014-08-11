#include "Hud.h"

extern Game* CurrentGame;

PlayerHud::PlayerHud()
{
	this->no_hazard_value = false;
}

void PlayerHud::Init(int m_armor, int m_shield)
{
	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_armor));
	armorBar.setFillColor(sf::Color(0, 250, 50));//green
	armorBar.setOutlineThickness(4);
	armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(0, 0);
	armorBar.setPosition(20, 10);

	shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1+m_shield));
	shieldBar.setFillColor(sf::Color(0, 50, 250));//blue
	shieldBar.setOutlineThickness(4);
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

		Money.setFont(*font);
		Money.setCharacterSize(20);
		Money.setColor(sf::Color::White);
		Money.setPosition(50,WINDOW_RESOLUTION_Y-50);

		GrazeScore.setFont(*font);
		GrazeScore.setCharacterSize(14);
		GrazeScore.setColor(sf::Color::White);
		GrazeScore.setPosition(50,WINDOW_RESOLUTION_Y-70);

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
	hazardBar.setFillColor(sf::Color(250, 0, 50));//red
	hazardBar.setOutlineThickness(4);
	hazardBar.setOutlineColor(sf::Color(255, 255, 255));
	hazardBar.setOrigin(0, 0);
	hazardBar.setPosition(WINDOW_RESOLUTION_X-40, 10);

	//hazardBarMax.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, hazard_break_value));
	hazardBarMax.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
	hazardBarMax.setFillColor(sf::Color::Transparent);//black
	hazardBarMax.setOutlineThickness(4);
	hazardBarMax.setOutlineColor(sf::Color(255, 255, 255));
	hazardBarMax.setOrigin(0, 0);
	hazardBarMax.setPosition(WINDOW_RESOLUTION_X-40, 10);
}

void PlayerHud::Update(sf::RenderWindow* window, int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_score, int m_hazard_break_value, sf::Time deltaTime)
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

	//hazard break
	if (m_hazard_break_value != 0)
	{
		hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, (m_hazard_score*HAZARD_BAR_SIZE_Y) / m_hazard_break_value));
	}
		
	else
	{
		no_hazard_value = true;
		printf("DEBUG: <error> HazardBar cannot be computed because hazard break value = 0)\n");
	}
		

	ostringstream ss_h1;
	ss_h1 << m_hazard_score;

	ostringstream ss_h2;
	ss_h2 << m_hazard_break_value;

	if (m_hazard_score > m_hazard_break_value) // max constraint
	{
		hazardBar.setSize(sf::Vector2f(HAZARD_BAR_SIZE_X, HAZARD_BAR_SIZE_Y));
		ss_h1 << m_hazard_break_value;
	}

	hazardBreakScore->setString(ss_h1.str() + "/" + ss_h2.str());

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
	window->draw(hazardBarMax);
	if (!no_hazard_value)
	{
		window->draw(hazardBar);
		window->draw(*(hazardBreakText));
		window->draw(*(hazardBreakScore));
	}
	
	window->draw(*(framerate));
}