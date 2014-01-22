#include "Hud.h"

PlayerHud::PlayerHud()
{
}

void PlayerHud::Init(int m_armor, int m_shield)
{
	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_armor));
	armorBar.setFillColor(sf::Color(0, 250, 50));//green
	armorBar.setOutlineThickness(4);
	armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(0, 0);
	armorBar.setPosition(20, 10);

	shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_shield));
	shieldBar.setFillColor(sf::Color(0, 50, 250));//blue
	shieldBar.setOutlineThickness(4);
	shieldBar.setOutlineColor(sf::Color(255, 255, 255));
	shieldBar.setOrigin(0,0);
	shieldBar.setPosition((40+ARMOR_BAR_SIZE_X),10);

	
}

void PlayerHud::update(int m_armor, int m_shield)
{
	if (m_armor <=0)
	{
		armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1));
	}
	else
	{
		armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_armor));
	}
	
	if (m_shield <=0) 
	{
		shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, 1));	
	}	
	else
	{
		shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_shield));	
	}	

}