#ifndef SFHUDPANEL_H_INCLUDED
#define SFHUDPANEL_H_INCLUDED

#include "SFInventoryPanel.h"

class SFHUDPanel : public SFInventoryPanel
{
public:
	SFHUDPanel(sf::Vector2f size, Ship* playership);
	~SFHUDPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	void UpdateUpgradeIcons() override;

	vector<GameObject> m_upgrades_icons;

	sf::RectangleShape m_progressionBar;
	sf::RectangleShape m_progressionExit;
	GameObject m_ship;
	sf::Text m_progression_text;

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

	GameObject m_crystal;
	sf::Text m_crystals_text;

	sf::RectangleShape m_black_background;

	//cached values, to avoid rebuilding text strings every frame when nothing changed
	int m_last_armor;
	int m_last_armor_max;
	int m_last_shield;
	int m_last_shield_max;
	int m_last_money;
	int m_last_crystals;
	string m_last_scene_name;
	int m_last_level;
};

#endif // SFHUDPANEL_H_INCLUDED