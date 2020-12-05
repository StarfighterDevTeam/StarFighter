#ifndef SFHUDPANEL_H_INCLUDED
#define SFHUDPANEL_H_INCLUDED

#include "SFInventoryPanel.h"

class SFHUDPanel : public SFInventoryPanel
{
public:
	SFHUDPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;

	sf::RectangleShape m_armorBar;
	sf::RectangleShape m_armorBarContainer;
	sf::RectangleShape m_shieldBar;
	sf::RectangleShape m_shieldBarContainer;
	sf::RectangleShape m_fuelBar;
	sf::RectangleShape m_fuelBarContainer;
	sf::RectangleShape m_fuelBarOverblock;
	sf::RectangleShape m_xpBar;
	sf::Text m_life_text;
	sf::Text m_shield_text;
	sf::Text m_fuel_text;
	sf::Text m_money_text;
	sf::Text m_level_text;
	sf::Text m_graze_text;
	sf::Text m_beastscore_text;
	sf::Text m_framerate_text;
	sf::Text m_scene_text;
	sf::Text m_hazardscore_text;
	sf::Text m_hitstaken_text;

	sf::Text m_combo_text;
	sf::RectangleShape m_comboBar;
	sf::RectangleShape m_comboBarContainer;

	sf::Text m_equipment_title;
	sf::Text m_inventory_title;
};

#endif // SFHUDPANEL_H_INCLUDED