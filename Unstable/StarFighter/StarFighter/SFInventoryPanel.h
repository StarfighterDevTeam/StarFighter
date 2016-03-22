#ifndef SFINVENTORYPANEL_H_INCLUDED
#define SFINVENTORYPANEL_H_INCLUDED

#include "Scene.h"

class SFItemStatsPanel : public SFPanel
{
public:
	SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, bool item_in_shop);
	~SFItemStatsPanel();
	void DisplayItemStats(GameObject* object);
	void Draw(sf::RenderTexture& screen) override;

	GameObject m_arrow;
	sf::Text m_options_text;
};

class SFInventoryPanel : public SFPanel
{
public:
	SFInventoryPanel(sf::Vector2f size, Ship* playerShip, size_t nb_lines, size_t nb_rows, bool use_two_grids, bool use_grey_if_no_money, size_t nb_lines2 = 0, size_t nb_rows2 = 0);
	~SFInventoryPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	GameObject* GetCursor() override;
	GameObject* GetFocusedItem() override;

	void SetFocusedItem(GameObject* item) override;
	sf::Vector2i GetFocusedIndex() override;
	int GetFocusedGrid() override;
	ObjectGrid* GetGrid(bool fake_grid = false, size_t grid = 1) override;
	bool GetPrioritaryFeedback() override;
	SFItemStatsPanel* GetItemStatsPanel() override;
	void SetItemStatsPanel(SFItemStatsPanel* panel) override;
	void SetPrioritaryFeedback(bool has_priotiary_feedback) override;
	void ClearHighlight() override;
	void ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid) override;

	SFInventoryPanel* Clone();

	GameObject* GetHoveredObjectInGrid(ObjectGrid grid, ObjectGrid fake_grid);
	GameObject* GetHoveredObjectInTwoGrids(ObjectGrid grid, ObjectGrid fake_grid, ObjectGrid grid2, ObjectGrid fake_grid2);

	GameObject m_cursor;
	GameObject* m_focused_item;
	sf::Vector2i m_focused_cell_index;
	ObjectGrid m_grid;
	ObjectGrid m_fake_grid;
	ObjectGrid m_grid2;
	ObjectGrid m_fake_grid2;
	ObjectGrid m_grey_grid;
	bool m_use_two_grids;
	int m_focused_grid;
	SFItemStatsPanel* m_item_stats_panel;
	bool m_has_prioritary_feedback;
	bool m_use_grey_if_no_money;

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
	sf::Text m_framerate_text;
	sf::Text m_scene_text;
};

#endif // SFINVENTORYPANEL_H_INCLUDED