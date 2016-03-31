#ifndef SFPANEL_H_INCLUDED
#define SFPANEL_H_INCLUDED

#include "HudGrid.h"
#include "Includes/SimpleCollision.hpp"

class Ship;
class Dialog;
class SFItemStatsPanel;

enum SFPanelTypes
{
	SFPanel_None,
	SFPanel_ItemStats,
	SFPanel_Inventory,
	SFPanel_DoubleInventory,
	SFPanel_Action,
	SFPanel_Portal,
	SFPanel_Shop,
	SFPanel_Dialog,
	SFPanel_DialogNext,
	SFPanel_Map,
	SFPanel_MapInfo,
	SFPanel_Trade,
};

enum TradeGrids
{
	Trade_ShopGrid,
	Trade_EquippedGrid,
	Trade_StashGrid,
	NBVAL_TradeGrids//3
};

enum FocusedItemStates
{
	FocusedItem_Equip,
	FocusedItem_Desequip,
	FocusedItem_Buy,
	FocusedItem_SellOrEquip,
	FocusedItem_SellOrDesequip,
};

class SFActionBox : public sf::RectangleShape
{
public:
	SFActionBox(sf::Vector2f position, string action1, string action2, string action3, string action4);
	void Draw(sf::RenderTexture& screen);

	sf::RectangleShape m_boxes[4];
	sf::Text m_texts[4];
};

class SFPanel : public sf::RectangleShape
{
public:
	SFPanel(){};
	SFPanel(sf::Vector2f size, SFPanelTypes panel_type);
	~SFPanel();
	virtual void Update(sf::Time deltaTime, sf::Vector2f inputs_directions);
	virtual void Draw(sf::RenderTexture& screen);
	static bool IsCursorCollidingWithRectangle(GameObject& cursor, RectangleShape& object);
	
	sf::Text m_title_text;
	sf::Text m_text;
	sf::Text m_actions_text;
	bool m_visible;
	SFPanelTypes m_panel_type;
	size_t m_number_of_options;

	Ship* m_playerShip;

	virtual void SetSelectedOptionIndex(int index);
	virtual int GetSelectedOptionIndex();
	virtual GameObject* GetCursor();
	virtual GameObject* GetFocusedItem();
	virtual void SetFocusedItem(GameObject* item);
	virtual ObjectGrid* GetGrid(bool fake_grid = false, int grid = 1);
	virtual sf::Vector2i GetFocusedIndex();
	virtual int GetFocusedIntIndex();
	virtual int GetFocusedGrid();
	virtual bool GetPrioritaryFeedback();
	virtual void SetPrioritaryFeedback(bool has_priotiary_feedback);
	virtual string GetTeleportationDestination();
	virtual int GetTeleportationCost();
	virtual void SetMapViewOffset(sf::Vector2f offset);
	virtual SFItemStatsPanel* GetItemStatsPanel();
	virtual void SetItemStatsPanel(SFItemStatsPanel* panel);
	virtual float GetDuration();
	virtual float GetDurationClockElpased();
	virtual int GetItemsStatsPanelIndex();
	virtual void SetItemsStatsPanelIndex(int index);
	virtual int GetItemsStatsPanelNumberOfOptions();
	
	virtual Dialog* GetDialog();

	virtual void ClearHighlight();
	virtual void ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid);
};

#endif // SFPANEL_H_INCLUDED