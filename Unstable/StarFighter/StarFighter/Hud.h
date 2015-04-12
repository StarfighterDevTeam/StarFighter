#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Independant.h"
#include "Includes/SimpleCollision.hpp"

#include "HudGrid.h"

class PlayerHud
{
public:
	PlayerHud();
	void Init(int m_armor, int m_shield);
	void Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub,
		int focused_item_type = -1, string f_name = "", float f_max_speed = 0, float f_hyperspeed = 1, int f_armor = 1, int f_shield = 0, int f_shield_regen = 0, int f_damage = 0, bool f_bot = false, float f_ammo_speed = 0, PatternType f_pattern = NoMovePattern,
		int f_multishot = 1, int f_xspread = 0, float f_rate_of_fire = 1, ShotMode f_shot_mode = NoShotMode, float f_dispersion = 0, int f_rafale = 0, float f_rafale_cooldown = 0, TargetSeaking f_target_seaking = NO_SEAKING);
	void Draw(sf::RenderTexture& offscreen);

	void GarbageObjectInGrid(int grid_id, int index);

	ObjectGrid fakeEquipmentGrid;
	ObjectGrid equipmentGrid;
	ObjectGrid fakeShipGrid;
	ObjectGrid shipGrid;

	sf::RectangleShape backgroundColor;
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
	//sf::Text ShipGridTitle;
	//sf::Text EquipmentGridTitle;
	sf::Text Money;
	sf::Text GrazeScore;
	sf::Text SceneName;
	sf::Font* font;
	sf::Font* font2;
	sf::Text* framerate;

	//item stats
	sf::RectangleShape itemStatsPanel;
	sf::Text itemStatsText;

	Independant* hud_cursor;
	Independant* focused_item;
	sf::Vector2i focused_grid_and_index;
	bool removing_item;

	bool max_hazard_level_reached;
	bool has_shield;
	bool has_focus;
};

#endif // HUD_H_INCLUDED