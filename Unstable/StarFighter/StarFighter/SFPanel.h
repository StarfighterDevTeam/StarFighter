#ifndef SFPANEL_H_INCLUDED
#define SFPANEL_H_INCLUDED

#include "HudGrid.h"
#include "Includes/SimpleCollision.hpp"

class Ship;
class Dialog;

enum SFPanelTypes
{
	SFPanel_None,
	SFPanel_ItemStats,
	SFPanel_Inventory,
	SFPanel_DoubleInventory,
	SFPanel_Portal,
	SFPanel_Shop,
	SFPanel_Dialog,
	SFPanel_DialogNext,
	SFPanel_Map,
	SFPanel_MapInfo,
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

	Ship* m_playerShip;

	virtual void SetSelectedOptionIndex(int index);
	virtual int GetSelectedOptionIndex();
	virtual GameObject* GetCursor();
	virtual GameObject* GetFocusedItem();
	virtual void SetFocusedItem(GameObject* item);
	virtual ObjectGrid* GetGrid(bool fake_grid = false, size_t grid = 1);
	virtual sf::Vector2i GetFocusedIndex();
	virtual int GetFocusedGrid();
	virtual bool GetPrioritaryFeedback();
	virtual void SetPrioritaryFeedback(bool has_priotiary_feedback);
	
	virtual Dialog* GetDialog();

	virtual void ClearHighlight();
	virtual void ForceCursorOnEquivalentObjectInGrid(GameObject* focused_object, ObjectGrid* fake_grid);
};

#endif // SFPANEL_H_INCLUDED