#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "GameObject.h"
#include "Includes/SimpleCollision.hpp"

#include "InteractionPanel.h"
#include "HudGrid.h"

class PlayerHud
{
public:
	PlayerHud();
	void Init(int armor, int shield, int xp, int xp_max);
	void Update(int armor, int armor_max, int shield, int shield_max, int money, int graze_count, int hazard_level, std::string scene_name, int level, int level_max, int xp, int xp_max, sf::Time deltaTime, bool hub,
		int focused_item_type = -1, string f_name = "", int f_level = 1, int f_xp = 0, float f_max_speed = 0, float f_hyperspeed = 1, int f_armor = 1, int f_shield = 0, int f_shield_regen = 0,
		int f_damage = 0, bool f_bot = false, float f_ammo_speed = 0, PatternType f_pattern = NoMovePattern,
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
	sf::RectangleShape armorBarContainer;
	sf::RectangleShape shieldBarContainer;
	sf::Text LifeText;
	sf::Text ShieldText;

	sf::RectangleShape xpBar;
	//sf::Text ShipGridTitle;
	//sf::Text EquipmentGridTitle;
	sf::Text Money;
	sf::Text GrazeScore;
	sf::Text SceneName;
	sf::Font* font;
	sf::Font* font2;
	sf::Text* framerate;

	sf::Text Level;

	//item stats
	sf::RectangleShape itemStatsPanel;
	sf::Text itemStatsText;

	GameObject* hud_cursor;
	GameObject* focused_item;
	sf::Vector2i focused_grid_and_index;
	void setRemovingCursorAnimation(CursorFeedbackStates animation_index);
	bool has_prioritary_cursor_feedback;

	bool max_hazard_level_reached;
	bool has_shield;
	bool has_focus;
};

#endif // HUD_H_INCLUDED