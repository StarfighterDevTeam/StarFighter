#include "Hud.h"

PlayerHud::PlayerHud()
{
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

	sf::Font* font = new sf::Font();
		if (!font->loadFromFile("Assets/Fonts/terminator_real_nfi.ttf"))
		{
			// error
			//TODO: font loader
		}

	HazardScore.setFont(*font);
	HazardScore.setCharacterSize(20);
	HazardScore.setColor(sf::Color::White);
	HazardScore.setPosition(50,WINDOW_RESOLUTION_Y-50);

	GrazeScore.setFont(*font);
	GrazeScore.setCharacterSize(14);
	GrazeScore.setColor(sf::Color::White);
	GrazeScore.setPosition(50,WINDOW_RESOLUTION_Y-70);
}

void PlayerHud::update(int m_armor, int m_shield, int m_money, int m_graze_count)
{
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

	ostringstream ss;
	ss << m_money;
	HazardScore.setString(ss.str()+"$");

	ss << m_graze_count;
	GrazeScore.setString("Graze: "+ss.str());
}
