#ifndef INTERACTIONPANEL_H_INCLUDED
#define INTERACTIONPANEL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "HudGrid.h"

enum ShopOptions
{
	ShopHeal,
	ShopBuy,
	StellarMap,
	NBVAL_ShopOptions,
};

enum ShopBuyOptions
{
	ShopBuy_Buy,
	ShopBuy_SwitchToSell,
	NBVAL_ShopBuyOptions,
};

enum ShopMenus
{
	ShopMainMenu,
	ShopBuyMenu,
	NBVAL_ShopMenus,
};

struct InteractionPanel
{
public:
	InteractionPanel();
	void SetPanelPosition(sf::Vector2f position);
	sf::Vector2f GetShopBuyPanelSize();

	void UpdatePortalInteraction(int max_unlocked_hazard_level);
	GameObject* UpdateShopInteraction(sf::Time deltaTime);
	GameObject* Update(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime);
	void InitCursorOnGrid();
	void Draw(sf::RenderTexture& screen);

	static void UpdateItemStatsText(sf::Text* text, int focused_item_type = -1, string f_name = "", int f_level = 1, int f_xp = 0, float f_max_speed = 0, float f_hyperspeed = 1, int f_armor = 1, int f_shield = 0, int f_shield_regen = 0,
		int f_damage = 0, bool f_bot = false, float f_ammo_speed = 0, PatternType f_pattern = NoMovePattern,
		int f_multishot = 1, int f_xspread = 0, float f_rate_of_fire = 1, ShotMode f_shot_mode = NoShotMode, float f_dispersion = 0, int f_rafale = 0, float f_rafale_cooldown = 0, TargetSeaking f_target_seaking = NO_SEAKING);

	sf::RectangleShape m_panel;
	sf::Text m_text[NB_HAZARD_LEVELS];
	sf::Text m_textHelpNavigation;
	sf::Text m_textHelpBuy;
	sf::Text m_textDestination;
	sf::Text m_textShopOptions[NBVAL_ShopOptions];
	sf::Font* m_font;
	GameObject* m_arrow;
	int m_selected_index;
	Directions m_direction;
	InteractionType m_currentInteractionType;
	ShopMenus m_currentShopMenu;

	sf::RectangleShape m_itemStatsPanel;
	sf::Text m_itemStatsText;

	ObjectGrid m_fakeShopGrid;
	ObjectGrid m_shopGrid;
	GameObject* m_cursor;
	GameObject* m_focused_item;
	sf::Vector2i m_focused_index;
};

#endif //INTERACTIONPANEL_H_INCLUDED