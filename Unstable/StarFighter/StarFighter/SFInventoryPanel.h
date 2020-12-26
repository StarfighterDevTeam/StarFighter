#ifndef SFINVENTORYPANEL_H_INCLUDED
#define SFINVENTORYPANEL_H_INCLUDED

#include "SFItemStatsPanel.h"

class SFInventoryPanel : public SFPanel
{
public:
	SFInventoryPanel(sf::Vector2f size, Ship* playership, SFPanelTypes panel_type);
	~SFInventoryPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	void DrawItemLevels(sf::RenderTexture& screen);
	void SetItemStatsPanel(SFItemStatsPanel* panel) override;
	int GetItemsStatsPanelIndex() override;
	void SetItemsStatsPanelIndex(int index) override;

	void SetCursorVisible_v2(bool visible) override;

	GameObject m_cursor;
	SFItemStatsPanel* m_item_stats_panel;
	SFItemStatsPanel* m_item_stats_panel_compare;
	bool m_has_prioritary_feedback;

	Ship* m_playership;

	//v2
	Grid* m_grids_v2[NBVAL_TradeGrids];

	void CreateGrids(SFPanel* panel, sf::Vector2f position0, sf::Vector2f position1, sf::Vector2f position2);
	GridElement* getCursorCollision(GameObject& cursor);
	GridElement* GetHighlightedElement() override;
	void SetHighlightedElement(GridElement* element) override;
	Grid* GetGrid_v2(int grid) override;
	GameObject* GetCursor() override;

	GridElement* m_highlighted_element;
};

#endif // SFINVENTORYPANEL_H_INCLUDED