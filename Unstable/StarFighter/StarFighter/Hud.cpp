#include "Hud.h"

PlayerHud::PlayerHud()
{
}

void PlayerHud::Init(int m_armor, int m_shield)
{
	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_armor));
	armorBar.setFillColor(sf::Color(0, 50, 250));
	armorBar.setOutlineThickness(4);
	armorBar.setOutlineColor(sf::Color(255, 255, 255));
	armorBar.setOrigin(ARMOR_BAR_SIZE_X, ARMOR_BAR_SIZE_Y);
	armorBar.setPosition(800 - 0.5*ARMOR_BAR_SIZE_X,600);

	shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_shield));
	shieldBar.setFillColor(sf::Color(100, 250, 50));
	shieldBar.setOutlineThickness(4);
	shieldBar.setOutlineColor(sf::Color(255, 255, 255));
	shieldBar.setOrigin(ARMOR_BAR_SIZE_X, ARMOR_BAR_SIZE_Y);
	shieldBar.setPosition(800 - 2.5*ARMOR_BAR_SIZE_X ,600);
}

void PlayerHud::update(int m_armor, int m_shield)
{
	armorBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_armor));
	shieldBar.setSize(sf::Vector2f(ARMOR_BAR_SIZE_X, m_shield));
}