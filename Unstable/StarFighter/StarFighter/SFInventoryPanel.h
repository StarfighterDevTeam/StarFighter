#ifndef SFINVENTORYPANEL_H_INCLUDED
#define SFINVENTORYPANEL_H_INCLUDED

#include "Scene.h"

class SFItemStatsPanel : public SFPanel
{
public:
	SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, FocusedItemStates item_state, bool comparison);
	~SFItemStatsPanel();
	void DisplayItemStats(GameObject* object);
	void Draw(sf::RenderTexture& screen) override;

	int m_selected_option_index;
	bool m_comparison;
	sf::RectangleShape m_overblock;
	sf::Text m_title_text_comparison;
};

class SFInventoryPanel : public SFPanel
{
public:
	SFInventoryPanel(sf::Vector2f size, Ship* playerShip, SFPanelTypes panel_type);
	~SFInventoryPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	GameObject* GetCursor() override;
	GameObject* GetFocusedItem() override;

	void SetFocusedItem(GameObject* item) override;
	sf::Vector2i GetFocusedIndex() override;
	int GetFocusedIntIndex() override;
	int GetFocusedGrid() override;
	ObjectGrid* GetGrid(bool fake_grid = false, int grid = 1) override;
	bool GetPrioritaryFeedback() override;
	SFItemStatsPanel* GetItemStatsPanel() override;
	void SetItemStatsPanel(SFItemStatsPanel* panel) override;
	void SetPrioritaryFeedback(bool has_priotiary_feedback) override;
	void ClearHighlight() override;
	void ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid) override;
	int GetItemsStatsPanelIndex() override;
	void SetItemsStatsPanelIndex(int index) override;

	SFInventoryPanel* Clone();
	static void UpdateBackgroundColors(ObjectGrid* color_grid, ObjectGrid* object_grid);
	static void UpdateGreyMaskOnInsufficientCredits(ObjectGrid* grey_grid, ObjectGrid* grid, Ship* playerShip);
	static void SetGridSlotsVisibility(ObjectGrid* grid, bool visible);
	static EquipmentQuality GetItemQualityClass(float quality);

	GameObject* GetHoveredObjectInGrid();
	GameObject* GetEquivalentEquippedItem(Ship* playerShip, GameObject* item);

	GameObject m_cursor;
	GameObject* m_focused_item;
	sf::Vector2i m_focused_cell_index;
	ObjectGrid* m_grid[NBVAL_TradeGrids];
	ObjectGrid* m_fake_grid[NBVAL_TradeGrids];
	ObjectGrid* m_quality_grid[NBVAL_TradeGrids];
	ObjectGrid* m_grey_grid[NBVAL_TradeGrids];

	int m_focused_grid;
	SFItemStatsPanel* m_item_stats_panel;
	SFItemStatsPanel* m_item_stats_panel_compare;
	bool m_has_prioritary_feedback;

	Ship* m_playerShip;
};

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

	sf::Text m_equipment_title;
	sf::Text m_inventory_title;
};

class SFTradePanel : public SFInventoryPanel
{
public:
	SFTradePanel(sf::Vector2f size, Ship* playerShip);
	void Draw(sf::RenderTexture& screen) override;

	sf::Text m_title_text2;
	sf::RectangleShape m_separator;
};

#endif // SFINVENTORYPANEL_H_INCLUDED